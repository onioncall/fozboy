const std = @import("std");
const testing = std.testing;
const c = @cImport({
    @cInclude("memory/mmu.h");
    @cInclude("cartridge/cart.h");
    @cInclude("cartridge/mbc.h");
    @cInclude("static/cart_type_data.h");
});

// Helper function to create a test cartridge
fn createTestCart(cart_type: c.cart_type_enum) c.cart_t {
    var cart: c.cart_t = std.mem.zeroes(c.cart_t);
    cart.cart_type = cart_type;
    cart.size = 32768; // 32KB
    cart.data = null; // No actual ROM data needed for these tests
    cart.is_ram = false;
    cart.is_batt = false;
    cart.ext_ram = c.ext_ram_create(cart_type);
    cart.mbc = c.mbc_create(cart_type);
    return cart;
}

// Helper function to clean up test cartridge
fn destroyTestCart(cart: *c.cart_t) void {
    if (cart.mbc != null) {
        c.mbc_destroy(cart.mbc);
        cart.mbc = null;
    }
    if (cart.ext_ram != null) {
        c.ext_ram_destroy(cart.ext_ram);
        cart.ext_ram = null;
    }
}

test "cart creation - basic" {
    var cart = createTestCart(c.MBC1);
    defer destroyTestCart(&cart);

    try testing.expect(cart.mbc != null);
    try testing.expect(cart.ext_ram != null);
}

test "mmu_create - basic initialization" {
    var cart = createTestCart(c.MBC1);
    defer destroyTestCart(&cart);
    const mmu = c.mmu_create(&cart);
    defer c.mmu_destroy(mmu);

    try testing.expect(mmu != null);
    try testing.expect(mmu.*.cart.*.mbc != null);
    try testing.expect(mmu.*.ram_enabled == false);
    try testing.expect(mmu.*.current_ram_bank == 0);
    try testing.expect(mmu.*.timer_enabled == false);
}

test "mmu_create - RTC initialization" {
    var cart = createTestCart(c.MBC3);
    defer destroyTestCart(&cart);
    const mmu = c.mmu_create(&cart);
    defer c.mmu_destroy(mmu);

    // Check RTC registers are initialized to 0
    try testing.expect(mmu.*.rtc_s == 0);
    try testing.expect(mmu.*.rtc_m == 0);
    try testing.expect(mmu.*.rtc_h == 0);
    try testing.expect(mmu.*.rtc_dl == 0);
    try testing.expect(mmu.*.rtc_dh == 0);

    // Check latched RTC registers are initialized to 0
    try testing.expect(mmu.*.rtc_s_latched == 0);
    try testing.expect(mmu.*.rtc_m_latched == 0);
    try testing.expect(mmu.*.rtc_h_latched == 0);
    try testing.expect(mmu.*.rtc_dl_latched == 0);
    try testing.expect(mmu.*.rtc_dh_latched == 0);
}

test "mmu_read - basic memory regions" {
    var cart = createTestCart(c.MBC1);
    defer destroyTestCart(&cart);
    const mmu = c.mmu_create(&cart);
    defer c.mmu_destroy(mmu);

    // Write test data to HRAM (0xFF80-0xFFFE)
    mmu.*.blocks[c.MMU_HRAM].*.buf[0] = 0x42;
    mmu.*.blocks[c.MMU_HRAM].*.buf[10] = 0xAB;

    // Read back the data
    const value1 = c.mmu_read(mmu, 0xFF80);
    const value2 = c.mmu_read(mmu, 0xFF8A);

    try testing.expect(value1 == 0x42);
    try testing.expect(value2 == 0xAB);
}

test "mmu_read - external RAM disabled returns 0xFF" {
    var cart = createTestCart(c.MBC1);
    defer destroyTestCart(&cart);
    const mmu = c.mmu_create(&cart);
    defer c.mmu_destroy(mmu);

    // External RAM is disabled by default
    try testing.expect(mmu.*.ram_enabled == false);

    // Reading from external RAM region should return 0xFF
    const value = c.mmu_read(mmu, 0xA000);
    try testing.expect(value == 0xFF);
}

test "mmu_write - basic memory write" {
    var cart = createTestCart(c.MBC1);
    defer destroyTestCart(&cart);
    const mmu = c.mmu_create(&cart);
    defer c.mmu_destroy(mmu);

    // Write to WRAM (0xC000-0xCFFF)
    c.mmu_write(mmu, 0xC000, 0x55);
    c.mmu_write(mmu, 0xC100, 0xAA);

    // Verify the writes
    try testing.expect(mmu.*.blocks[c.MMU_WRAM].*.buf[0] == 0x55);
    try testing.expect(mmu.*.blocks[c.MMU_WRAM].*.buf[0x100] == 0xAA);
}

test "mmu_write - external RAM disabled ignores writes" {
    var cart = createTestCart(c.MBC1);
    defer destroyTestCart(&cart);
    const mmu = c.mmu_create(&cart);
    defer c.mmu_destroy(mmu);

    // External RAM and timer are disabled by default
    try testing.expect(mmu.*.ram_enabled == false);
    try testing.expect(mmu.*.timer_enabled == false);

    // Try to write to external RAM region - should be ignored
    c.mmu_write(mmu, 0xA000, 0x77);

    // Reading should still return 0xFF (disabled)
    const value = c.mmu_read(mmu, 0xA000);
    try testing.expect(value == 0xFF);
}

test "switch_rom - invalid bank numbers" {
    var cart = createTestCart(c.MBC1);
    defer destroyTestCart(&cart);
    const mmu = c.mmu_create(&cart);
    defer c.mmu_destroy(mmu);

    // Test invalid bank numbers
    const result1 = c.switch_rom(mmu, 1, 0); // Bank 1 is invalid (< 2)
    const result2 = c.switch_rom(mmu, 513, 0); // Bank 513 is invalid (> 512)

    try testing.expect(result1 == -1);
    try testing.expect(result2 == -1);
}

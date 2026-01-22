const std = @import("std");
const testing = std.testing;
const c = @cImport({
    @cInclude("memory/mbc.h");
    @cInclude("memory/rom.h");
    @cInclude("static/cart_type_data.h");
});

// Helper function to create a test ROM
fn createTestRom(cart_type: c.cart_type_enum) c.rom_t {
    var rom: c.rom_t = std.mem.zeroes(c.rom_t);
    rom.cart_type = cart_type;
    return rom;
}

test "mbc1_intercept - RAM gate enable" {
    var rom = createTestRom(c.MBC1);
    const mbc = c.mbc_create(&rom);
    defer c.mbc_destroy(mbc);

    // Test RAM gate enable (write 0x0A to address 0x0000-0x1FFF)
    const flags = mbc.*.intercept.?(mbc, 0x1000, 0x0A);

    try testing.expect(flags.mbc == true);
    try testing.expect(flags.set_ram_gate == true);
    try testing.expect(flags.ram_gate_enabled == true);
}

test "mbc1_intercept - RAM gate disable" {
    var rom = createTestRom(c.MBC1);
    const mbc = c.mbc_create(&rom);
    defer c.mbc_destroy(mbc);

    // Test RAM gate disable (write something other than 0x0A)
    const flags = mbc.*.intercept.?(mbc, 0x1000, 0x00);

    try testing.expect(flags.mbc == true);
    try testing.expect(flags.set_ram_gate == true);
    try testing.expect(flags.ram_gate_enabled == false);
}

test "mbc1_intercept - BANK1 register write" {
    var rom = createTestRom(c.MBC1);
    const mbc = c.mbc_create(&rom);
    defer c.mbc_destroy(mbc);

    // Test BANK1 register write (address 0x2000-0x3FFF)
    const flags = mbc.*.intercept.?(mbc, 0x2000, 0x05);

    try testing.expect(flags.mbc == true);
    try testing.expect(flags.set_switch_bank == true);
    try testing.expect(flags.switch_bank == 0x05);
    try testing.expect(mbc.*.regs.*.bank1 == 0x05);
}

test "mbc1_intercept - BANK1 register zero handling" {
    var rom = createTestRom(c.MBC1);
    const mbc = c.mbc_create(&rom);
    defer c.mbc_destroy(mbc);

    // MBC1 treats 0 as 1 for BANK1 register
    const flags = mbc.*.intercept.?(mbc, 0x2000, 0x00);

    try testing.expect(flags.mbc == true);
    try testing.expect(flags.set_switch_bank == true);
    try testing.expect(flags.switch_bank == 0x01);
    try testing.expect(mbc.*.regs.*.bank1 == 0x01);
}

test "mbc1_intercept - BANK2 register write" {
    var rom = createTestRom(c.MBC1);
    const mbc = c.mbc_create(&rom);
    defer c.mbc_destroy(mbc);

    // Set BANK1 first
    _ = mbc.*.intercept.?(mbc, 0x2000, 0x05);

    // Test BANK2 register write (address 0x4000-0x5FFF)
    const flags = mbc.*.intercept.?(mbc, 0x4000, 0x02);

    try testing.expect(flags.mbc == true);
    try testing.expect(flags.set_switch_bank == true);
    try testing.expect(mbc.*.regs.*.bank2 == 0x02);
    // BANK2 is shifted left by 5 bits and OR'd with BANK1
    try testing.expect(flags.switch_bank == ((0x02 << 5) | 0x05));
}

test "mbc1_intercept - mode switch to mode 1" {
    var rom = createTestRom(c.MBC1);
    const mbc = c.mbc_create(&rom);
    defer c.mbc_destroy(mbc);

    // Set BANK2 first
    _ = mbc.*.intercept.?(mbc, 0x4000, 0x02);

    // Switch to mode 1 (address 0x6000-0x7FFF)
    const flags = mbc.*.intercept.?(mbc, 0x6000, 0x01);

    try testing.expect(flags.mbc == true);
    try testing.expect(flags.set_fixed_bank == true);
    try testing.expect(flags.fixed_bank == (0x02 << 5));
    try testing.expect(mbc.*.regs.*.mode == 0x01);
}

test "mbc1_intercept - address out of range" {
    var rom = createTestRom(c.MBC1);
    const mbc = c.mbc_create(&rom);
    defer c.mbc_destroy(mbc);

    // Address > 0x7FFF should return empty flags
    const flags = mbc.*.intercept.?(mbc, 0x8000, 0xFF);

    try testing.expect(flags.mbc == false);
}

test "mbc_create - MBC1 initialization" {
    var rom = createTestRom(c.MBC1);
    const mbc = c.mbc_create(&rom);
    defer c.mbc_destroy(mbc);

    try testing.expect(mbc != null);
    try testing.expect(mbc.*.regs != null);
    // MBC1 should initialize BANK1 to 1
    try testing.expect(mbc.*.regs.*.bank1 == 0x01);
}

const std = @import("std");

// Although this function looks imperative, note that its job is to
// declaratively construct a build graph that will be executed by an external
// runner.
pub fn build(b: *std.Build) void {
    // what target to build for. Here we do not override the defaults, which
    // means any target is allowed, and the default is native. Other options
    // for restricting supported target set are available.
    const target = b.standardTargetOptions(.{});

    // Standard optimization options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall. Here we do not
    // set a preferred release mode, allowing the user to decide how to optimize.
    const optimize = b.standardOptimizeOption(.{});

    // For C-only projects, we need to explicitly create a module first.
    // For Zig projects, the module is created implicitly when you provide root_source_file.
    const lib_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const core_c_files = [_][] const u8{ 
        "emulator/gbc.c",
        "emulator/cpu/cpu.c",
        "emulator/memory/mmu.c",
        "emulator/memory/rom.c"
    };

    // Add C source files to the module (not needed for Zig projects)
    for (core_c_files) |file_name| {
        lib_module.addCSourceFile(.{
            .file = b.path(file_name),
            .flags = &.{ "-std=c11", "-fno-sanitize=undefined" },
        });
    }

    lib_module.addIncludePath(b.path("emulator"));

    // This was annoying, zig init creates a build file with addStaticLibrary instead, which is outdated
    // even for my version of zig (0.15.1) https://github.com/ziglang/zig/pull/22554
    const lib = b.addLibrary(.{
        .name = "gbc",
        .linkage = .static,
        .root_module = lib_module,
    });
    lib.linkLibC();

    b.installArtifact(lib);

    // Install header files so they're accessible to other languages (in our case, CGO)
    // This is C-specific. This only needs to be done for files we are exposing to go.
    const install_main_header = b.addInstallFile(b.path("emulator/gbc.h"), "include/gbc.h");
    b.getInstallStep().dependOn(&install_main_header.step);
}


fn main() {
    // This will consider the ffi part in interfaces_rs.rs in order to
    // generate interfaces_rs.rs.h and interfaces_rs.rs.cc
    // This will include the library required to import the Rust library
    cxx_build::bridge("src/interface_rs.rs")
        .compile("testlib_interface");
}
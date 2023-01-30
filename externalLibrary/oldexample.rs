use sha2::{Sha256,Digest};
use std::ffi::CString;
use std::ffi::CStr;
use std::os::raw::c_char;

// This is a minimal example which is memory unsafe and should not be used. I used it to test the build process.
// With `cbindgen --crate testlib --output ../HowToImport.h` I generated a header file which I used in the C++ code.

#[no_mangle]
pub unsafe extern "C" fn rust_function(s:*const c_char) -> *const c_char  {
    let mut hasher = Sha256::new();
    hasher.update(CStr::from_ptr(s).to_str().unwrap());
    let hash = hasher.finalize();
    let return_value = CString::new(format!("{:x}", hash)).unwrap();
    return return_value.into_raw();
}

#[no_mangle]
pub extern "C" fn hello_world()  {
    println!("Hello world from Rust shared library!\n");
}



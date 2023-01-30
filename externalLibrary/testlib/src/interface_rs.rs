// This specifies the bridge between Rust and C++
use cxx::CxxString;
use sha2::{Sha256,Digest};
use std::pin::Pin;

#[cxx::bridge]
mod ffi {
    extern "Rust" {
        fn hello_world() -> ();
    }
    extern "Rust" {
        fn rust_function(input:&CxxString,output:Pin<& mut CxxString>) -> i32; // Test function which does a sha256 and returns 0
    }
}

pub fn hello_world() -> () {
    println!("Hello world from Rust shared library!");
}
pub fn rust_function(input:&CxxString,output:Pin<& mut CxxString>) -> i32 {
    let s = String::from(input.to_string_lossy());
    let mut hasher = Sha256::new();
    let is_empty=s.len() == 0;
    hasher.update(s);
    let hash = hasher.finalize();
    output.push_str(format!("{:x}", hash).as_str());
    if is_empty {
        return -1;
    }
    return 0;
}
use std::fs::File;
use std::io::ErrorKind;
use std::io::{self, BufRead};
use std::path::Path;

pub type RawLines = Vec<String>;

pub fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

pub fn get_input_file() -> io::Result<String> {
    let args: Vec<String> = std::env::args().collect();

    if args.len() > 1 {
        Ok(args[1].clone())
    } else if Path::new("input.txt").exists() {
        Ok("input.txt".to_string())
    } else {
        Err(io::Error::new(
            ErrorKind::NotFound,
            "No input file specified and input.txt not found in the current directory.",
        ))
    }
}

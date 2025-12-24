use std::fs::File;
use std::io::{self, BufRead};

pub type RawLines = Vec<String>;

pub fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

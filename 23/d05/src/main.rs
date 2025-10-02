use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

type Element = usize;

struct SubMap {
    input_start: Element,
    output_start: Element,
    length: Element,
}

impl SubMap {
    fn from_line(line: &str) -> Self {
        let components = line.split(' ').collect::<Vec<&str>>();
        match components[..] {
            [output_start, input_start, length] => SubMap {
                input_start: input_start.parse::<Element>().expect("Bad formatting"),
                output_start: output_start.parse::<Element>().expect("Bad formatting"),
                length: length.parse::<Element>().expect("Bad formatting"),
            },
            _ => panic!("Bad formatting!"),
        }
    }

    fn try_translate(&self, input: Element) -> Option<Element> {
        if input >= self.input_start && input < self.input_start + self.length {
            Some(self.output_start + (input - self.input_start))
        } else {
            None
        }
    }

    // fn print(&self) {
    //     println!(
    //         "Map: [{}:{}] -> [{}:{}]",
    //         self.input_start,
    //         self.input_start + self.length - 1,
    //         self.output_start,
    //         self.output_start + self.length - 1
    //     );
    // }
}

struct TranslatorMap {
    maps: Vec<SubMap>,
}

impl TranslatorMap {
    fn from_lines(mut lines: RawLines) -> Self {
        let sub_map_lines = lines.split_off(1);
        let mut sub_maps = sub_map_lines
            .iter()
            .map(|line| SubMap::from_line(line))
            .collect::<Vec<SubMap>>();
        sub_maps.sort_by_key(|elem| elem.input_start);
        TranslatorMap { maps: sub_maps }
    }

    fn translate(&self, input: Element) -> Element {
        for sub_map in &self.maps {
            if let Some(output) = sub_map.try_translate(input) {
                return output;
            }
        }
        input
    }
}

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

fn read_seeds(line: &str) -> Vec<Element> {
    let nums = line.strip_prefix("seeds: ").expect("Bad format");
    nums.split(' ')
        .map(|number| number.parse::<Element>().expect("Shouldn't fail"))
        .collect::<Vec<Element>>()
}

fn get_filter_lines(data: RawLines) -> Vec<RawLines> {
    let mut filters = vec![];
    let mut current_filter = vec![];
    for line in data.into_iter() {
        if line.is_empty() {
            filters.push(current_filter);
            current_filter = vec![];
            continue;
        }
        current_filter.push(line);
    }
    if !current_filter.is_empty() {
        filters.push(current_filter);
    }
    filters
}

fn main() -> std::io::Result<()> {
    let filepath = "input.txt".to_string();
    let raw_lines: RawLines = read_from_file(&filepath)?;
    println!("Found {} lines in {filepath}", raw_lines.len());

    let seeds = read_seeds(&raw_lines[0]);
    let raw_maps = get_filter_lines(raw_lines[2..].to_vec());

    let maps = raw_maps
        .into_iter()
        .map(TranslatorMap::from_lines)
        .collect::<Vec<TranslatorMap>>();

    let mut locations = vec![];
    // let mut seed_ranges = seeds.clone();
    for seed in seeds {
        let mut output = seed;
        for map in &maps {
            output = map.translate(output);
        }
        locations.push(output);
    }

    println!(
        "P1 location: {}",
        locations.iter().min().expect("Must be a min")
    );

    // let mut location = Element::MAX;
    // while !seed_ranges.is_empty() {
    //     let length = seed_ranges.pop().expect("Pairs");
    //     let range_start = seed_ranges.pop().expect("Pairs");
    //     println!(
    //         "{} -> {}: {} elements",
    //         range_start,
    //         range_start + length,
    //         length,
    //     );
    //     for seed in range_start..range_start + length {
    //         let mut output = seed;
    //         for map in &maps {
    //             output = map.translate(output);
    //         }
    //         if output < location {
    //             location = output;
    //         }
    //     }
    // }

    // println!("P2 location: {}", location);

    Ok(())
}

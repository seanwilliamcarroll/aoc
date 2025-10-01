use std::collections::HashSet;
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

struct Game {
    _id: u32,
    winning_numbers: Vec<u32>,
    drawn_numbers: Vec<u32>,
}

impl Game {
    fn from_str(line: &str) -> Game {
        let (id_string, remainder) = line.split_once(':').expect("Bad formatting");
        let (winning_numbers, drawn_numbers) = remainder.split_once('|').expect("Bad formatting");
        fn parse_nums(input: &str) -> Vec<u32> {
            input
                .split(' ')
                .filter(|word| !word.is_empty())
                .map(|number| number.parse::<u32>().expect("Bad formatting"))
                .collect()
        }
        let (_, id_string) = id_string.split_once("Card ").expect("Bad formatting");
        let id = id_string
            .trim_start()
            .parse::<u32>()
            .expect("Bad formatting");
        let winning_numbers: Vec<u32> = parse_nums(winning_numbers);
        let drawn_numbers: Vec<u32> = parse_nums(drawn_numbers);
        Game {
            _id: id,
            winning_numbers,
            drawn_numbers,
        }
    }

    fn score_game(&self) -> u32 {
        let winning_numbers: HashSet<u32> =
            HashSet::from_iter(self.winning_numbers.iter().cloned());
        let num_matches: u32 = self
            .drawn_numbers
            .iter()
            .map(|num| if winning_numbers.contains(num) { 1 } else { 0 })
            .sum();
        if num_matches == 0 {
            0u32
        } else {
            2u32.pow(num_matches - 1)
        }
    }
}

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

fn main() -> std::io::Result<()> {
    let filepath = "input.txt".to_string();
    let raw_lines = read_from_file(&filepath)?;
    println!("Found {} games in {filepath}", raw_lines.len());
    let games: Vec<Game> = raw_lines
        .iter()
        .map(|game_str| Game::from_str(&game_str))
        .collect();
    let summed_scores: u32 = games.iter().map(Game::score_game).sum();
    println!("Sum P1: {summed_scores}");
    Ok(())
}

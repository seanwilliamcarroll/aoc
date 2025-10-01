use std::collections::HashSet;
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

struct Game {
    winning_numbers: Vec<usize>,
    drawn_numbers: Vec<usize>,
}

impl Game {
    fn from_str(line: &str) -> Game {
        fn parse_nums(input: &str) -> Vec<usize> {
            input
                .split(' ')
                .filter(|word| !word.is_empty())
                .map(|number| number.parse::<usize>().expect("Bad formatting"))
                .collect()
        }
        let (_, remainder) = line.split_once(':').expect("Bad formatting");
        let (winning_numbers, drawn_numbers) = remainder.split_once('|').expect("Bad formatting");
        let winning_numbers: Vec<usize> = parse_nums(winning_numbers);
        let drawn_numbers: Vec<usize> = parse_nums(drawn_numbers);
        Game {
            winning_numbers,
            drawn_numbers,
        }
    }

    fn get_num_matches(&self) -> usize {
        let winning_numbers: HashSet<usize> = self.winning_numbers.iter().copied().collect();
        self.drawn_numbers
            .iter()
            .map(|num| usize::from(winning_numbers.contains(num)))
            .sum()
    }

    fn score_game(&self) -> usize {
        let num_matches = self.get_num_matches();
        if num_matches == 0 {
            0
        } else {
            2usize.pow(
                (num_matches - 1)
                    .try_into()
                    .expect("Shouldn't overflow here"),
            )
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
        .map(|game_str| Game::from_str(game_str))
        .collect();
    let summed_scores: usize = games.iter().map(Game::score_game).sum();
    println!("Sum P1: {summed_scores}");

    let mut card_counts: Vec<usize> = vec![1; games.len()];

    for (index, game) in games.iter().enumerate() {
        let num_matches = game.get_num_matches();
        for copied_card_index in index + 1..=index + num_matches {
            card_counts[copied_card_index] += card_counts[index];
        }
    }
    println!("Sum P2: {}", card_counts.iter().sum::<usize>());

    Ok(())
}

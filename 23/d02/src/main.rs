use std::fs::File;
use std::io::{self, BufRead};

type Selection = Option<u32>;

struct Draw {
    red: Selection,
    green: Selection,
    blue: Selection,
}

impl Draw {
    fn from_string(line: &str) -> Self {
        let selections: Vec<&str> = line.split(',').collect();
        let mut red: Selection = None;
        let mut green: Selection = None;
        let mut blue: Selection = None;
        for selection in selections {
            let selection = selection.trim();
            let (value, remaining) = selection.split_once(' ').expect("Bad selection");
            let value = value.parse::<u32>().expect("Should be a number here");
            if remaining.contains("red") {
                assert!(red.is_none());
                red = Some(value);
            } else if remaining.contains("green") {
                assert!(green.is_none());
                green = Some(value);
            } else if remaining.contains("blue") {
                assert!(blue.is_none());
                blue = Some(value);
            }
        }
        Draw { red, green, blue }
    }

    fn new() -> Self {
        Draw {
            red: None,
            green: None,
            blue: None,
        }
    }
}

struct Game {
    id: u32,
    draws: Vec<Draw>,
}

impl Game {
    fn from_string(line: &str) -> Self {
        let (game_id_string, remaining) = line.split_once(':').expect("Bad game format");
        let (_, id_string) = game_id_string.split_once("Game ").expect("Bad game format");
        let id = id_string.parse::<u32>().expect("Expected number here!");
        let remaining: Vec<&str> = remaining.split(';').collect();
        let draws: Vec<Draw> = remaining
            .into_iter()
            .map(|draw_string| Draw::from_string(draw_string))
            .collect();
        Game { id, draws }
    }

    fn max_draw(&self) -> Draw {
        let mut out_draw = Draw::new();
        for draw in &self.draws {
            if let Some(value) = draw.red {
                if out_draw.red.is_none() || value > out_draw.red.expect("Already checked") {
                    out_draw.red = draw.red;
                }
            }
            if let Some(value) = draw.green {
                if out_draw.green.is_none() || value > out_draw.green.expect("Already checked") {
                    out_draw.green = draw.green;
                }
            }
            if let Some(value) = draw.blue {
                if out_draw.blue.is_none() || value > out_draw.blue.expect("Already checked") {
                    out_draw.blue = draw.blue;
                }
            }
        }
        out_draw
    }
}

fn main() -> std::io::Result<()> {
    let file = File::open("input.txt")?;

    let mut sum_p1: u32 = 0;
    let mut sum_p2: u32 = 0;
    let reader = io::BufReader::new(file);
    for line in reader.lines() {
        let line = line?;
        let game = Game::from_string(&line);
        let max_draw = game.max_draw();
        match max_draw {
            Draw {
                red: Some(value), ..
            } if value > 12 => (),
            Draw {
                green: Some(value), ..
            } if value > 13 => (),
            Draw {
                blue: Some(value), ..
            } if value > 14 => (),
            _ => {
                sum_p1 += game.id;
            }
        }
        match max_draw {
            Draw {
                red: Some(red_val),
                blue: Some(blue_val),
                green: Some(green_val),
            } => {
                sum_p2 += red_val * blue_val * green_val;
            }
            _ => (),
        }
    }
    println!("P1: {}", sum_p1);
    println!("P2: {}", sum_p2);
    Ok(())
}

use common::{RawLines, Tile, get_input_file, read_from_file};

struct Packing {
    dims: (usize, usize),
    present_counts: Vec<usize>,
}

impl Packing {
    fn from_line(line: String) -> Self {
        let (dims_str, remaining_str) = line.split_once(':').expect("Bad formatting");

        let (row_str, col_str) = dims_str.split_once('x').expect("Bad formatting");
        let dims = (
            row_str.parse::<usize>().expect("Must work"),
            col_str.parse::<usize>().expect("Must work"),
        );

        let present_counts = remaining_str
            .trim()
            .split(' ')
            .map(|elem| elem.parse::<usize>().expect("Must work"))
            .collect::<Vec<usize>>();

        Self {
            dims,
            present_counts,
        }
    }

    fn area(&self) -> usize {
        self.dims.0 * self.dims.1
    }
}

struct TreeRegions {
    presents: Vec<Vec<Vec<Tile>>>,
    tree_areas: Vec<Packing>,
}

impl TreeRegions {
    fn from_raw_lines(lines: RawLines) -> Self {
        let mut presents = vec![];
        let mut tree_areas = vec![];

        let mut present = vec![];

        let mut present_mode = true;
        for line in lines {
            if line.contains('x') {
                present_mode = false;
            }
            if present_mode {
                if line.is_empty() {
                    presents.push(present.clone());
                    present.clear();
                } else if line.chars().nth(0).expect("Already checked") == '.'
                    || line.chars().nth(0).expect("Already checked") == '#'
                {
                    present.push(line.chars().collect::<Vec<Tile>>());
                }
            } else {
                tree_areas.push(Packing::from_line(line));
            }
        }

        Self {
            presents,
            tree_areas,
        }
    }

    fn regions_that_fit(&self) -> usize {
        let mut count = 0usize;

        let present_areas = self
            .presents
            .iter()
            .map(|region| {
                region
                    .iter()
                    .map(|row| row.iter().filter(|val| **val == '#').count())
                    .sum::<usize>()
            })
            .collect::<Vec<usize>>();

        for packing in &self.tree_areas {
            let area_to_fit = packing.area();

            let total_to_try = packing
                .present_counts
                .iter()
                .enumerate()
                .map(|(index, present_count)| present_count * present_areas[index])
                .sum::<usize>();

            if area_to_fit >= total_to_try {
                count += 1;
            }
        }

        count
    }
}

fn main() -> std::io::Result<()> {
    let filepath = get_input_file()?;
    let raw_lines = read_from_file(&filepath)?;

    println!("Found {} lines", raw_lines.len());

    let tree_regions = TreeRegions::from_raw_lines(raw_lines);

    println!("Found {} presents", tree_regions.presents.len());
    println!("Found {} tree_areas", tree_regions.tree_areas.len());

    let p1_solution = tree_regions.regions_that_fit();

    println!("P1 Solution: {p1_solution}");

    Ok(())
}

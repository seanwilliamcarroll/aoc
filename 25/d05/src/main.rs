use common::{RawLines, read_from_file};

type Unit = usize;

struct Range {
    lower: Unit,
    upper: Unit,
}

impl Range {
    fn from_line(line: &str) -> Self {
        let (lower_str, upper_str) = line.split_once('-').expect("Bad formatting");

        let lower = lower_str.parse::<Unit>().expect("Bad formatting");
        let upper = upper_str.parse::<Unit>().expect("Bad formatting");

        Self { lower, upper }
    }

    fn overlaps(&self, other: &Self) -> bool {
        // Probably overkill
        self.contains(other.lower)
            || self.contains(other.upper)
            || other.contains(self.lower)
            || other.contains(self.upper)
    }

    fn from_ranges(first: &Self, second: &Self) -> Option<Self> {
        if !first.overlaps(second) {
            return None;
        }

        Some(Self {
            lower: Unit::min(first.lower, second.lower),
            upper: Unit::max(first.upper, second.upper),
        })
    }

    fn contains(&self, value: Unit) -> bool {
        self.lower <= value && value <= self.upper
    }

    fn size(&self) -> usize {
        self.upper - self.lower + 1
    }
}

fn get_ranges_and_ids_from_raw_lines(lines: RawLines) -> (Vec<Range>, Vec<Unit>) {
    let mut ranges: Vec<Range> = vec![];
    let mut ids: Vec<Unit> = vec![];

    let mut finished_ranges = false;
    for line in lines.into_iter() {
        if !finished_ranges {
            if line.is_empty() {
                finished_ranges = true;
                continue;
            }
            ranges.push(Range::from_line(&line));
        } else {
            ids.push(line.parse::<Unit>().expect("Bad formatting"));
        }
    }
    (ranges, ids)
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let (mut ranges, ids) = get_ranges_and_ids_from_raw_lines(raw_lines);

    println!("{} ranges, {} ids", ranges.len(), ids.len());

    let mut p1_solution = 0usize;

    for id in &ids {
        for range in &ranges {
            if range.contains(*id) {
                p1_solution += 1;
                break;
            }
        }
    }

    println!("P1 Solution: {p1_solution}");

    ranges.sort_by_key(|range| range.lower);

    let mut merged_ranges: Vec<Range> = vec![];

    let mut current_range = ranges.remove(0);

    for other_range in ranges.into_iter() {
        if let Some(merged_range) = Range::from_ranges(&current_range, &other_range) {
            current_range = merged_range;
        } else {
            merged_ranges.push(current_range);
            current_range = other_range;
        }
    }
    merged_ranges.push(current_range);

    let p2_solution: usize = merged_ranges
        .into_iter()
        .map(|range| range.size())
        .sum::<usize>();

    println!("P2 Solution: {p2_solution}");

    Ok(())
}

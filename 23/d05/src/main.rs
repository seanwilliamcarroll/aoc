use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

type Element = usize;

#[derive(Clone, PartialEq, Eq, Debug)]
struct Range {
    pub start: Element,
    pub length: Element,
}

impl Range {
    fn safe_construct(start: Element, length: Element) -> Self {
        assert!(length != 0);
        Self { start, length }
    }

    fn from_magnitude(start: Element, length: Element) -> Self {
        Self::safe_construct(start, length)
    }

    #[allow(dead_code)]
    fn from_inclusive_range(start: Element, inclusive_end: Element) -> Self {
        Self::safe_construct(start, inclusive_end - start + 1)
    }

    fn from_exclusive_range(start: Element, exclusive_end: Element) -> Self {
        Self::safe_construct(start, exclusive_end - start)
    }

    fn exclusive_end(&self) -> Element {
        self.start + self.length
    }

    fn inclusive_end(&self) -> Element {
        self.exclusive_end() - 1usize
    }

    fn contains_point(&self, point: Element) -> bool {
        point >= self.start && point < self.exclusive_end()
    }

    fn does_overlap(&self, other: &Self) -> bool {
        self.contains_point(other.start) || other.contains_point(self.start)
    }

    fn intersection(&self, other: &Self) -> Option<Self> {
        if self.does_overlap(other) {
            let greatest_start = Element::max(self.start, other.start);
            let smallest_exclusive_end = Element::min(self.exclusive_end(), other.exclusive_end());
            let output = Self::from_exclusive_range(greatest_start, smallest_exclusive_end);
            Some(output)
        } else {
            None
        }
    }

    fn contains(&self, other: &Self) -> bool {
        // Does self completely contain the other range?
        self.does_overlap(other)
            && (self == other
                || self.start <= other.start && self.exclusive_end() >= other.exclusive_end())
    }

    fn difference(&self, other: &Self) -> Vec<Self> {
        // Could return 0, 1, or 2 ranges
        if !self.does_overlap(other) {
            vec![self.clone()]
        } else if self == other || other.contains(self) {
            vec![]
        } else if self.contains(other) {
            if self.start == other.start {
                vec![Self::from_exclusive_range(
                    other.exclusive_end(),
                    self.exclusive_end(),
                )]
            } else {
                vec![
                    Self::from_exclusive_range(self.start, other.start),
                    Self::from_exclusive_range(other.exclusive_end(), self.exclusive_end()),
                ]
            }
        } else if self.start < other.start {
            vec![Self::from_exclusive_range(self.start, other.start)]
        } else {
            // Self.start >= other.start
            vec![Self::from_exclusive_range(
                other.exclusive_end(),
                self.exclusive_end(),
            )]
        }
    }

    fn union(&self, other: &Self) -> Option<Self> {
        if self.does_overlap(other) {
            Some(Self::from_exclusive_range(
                Element::min(self.start, other.start),
                Element::max(self.exclusive_end(), other.exclusive_end()),
            ))
        } else {
            None
        }
    }

    fn union_all(mut ranges: Vec<Self>) -> Vec<Self> {
        ranges.sort_by_key(|range| range.start);
        ranges.reverse();

        let mut output = vec![];
        while ranges.len() >= 2 {
            let range_1 = ranges.pop().expect("Just checked this");
            let range_0 = ranges.pop().expect("Just checked this");

            if let Some(new_range) = range_0.union(&range_1) {
                ranges.push(new_range);
            } else {
                output.push(range_1);
                ranges.push(range_0);
            }
        }

        if let Some(range) = ranges.pop() {
            output.push(range);
        }

        output.sort_by_key(|range| range.start);
        output
    }
}

impl std::fmt::Display for Range {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "[{}, {}]", self.start, self.inclusive_end())
    }
}

#[derive(Debug)]
struct MapRange {
    input_start: Element,
    output_start: Element,
    length: Element,
}

impl MapRange {
    fn from_line(line: &str) -> Self {
        let components = line.split(' ').collect::<Vec<&str>>();
        match components[..] {
            [output_start, input_start, length] => MapRange {
                input_start: input_start.parse::<Element>().expect("Bad formatting"),
                output_start: output_start.parse::<Element>().expect("Bad formatting"),
                length: length.parse::<Element>().expect("Bad formatting"),
            },
            _ => panic!("Bad formatting!"),
        }
    }

    fn identity_map_from_range(range: &Range) -> Self {
        Self {
            input_start: range.start,
            output_start: range.start,
            length: range.length,
        }
    }

    fn try_translate_point(&self, input: Element) -> Option<Element> {
        if self.input_range().contains_point(input) {
            Some(self.output_start + (input - self.input_start))
        } else {
            None
        }
    }

    fn translate_range(&self, range: &Range) -> (Option<MapRange>, Vec<MapRange>) {
        let input_range = self.input_range();
        let mut output = vec![];
        let mut intersected = None;
        if let Some(overlapping_range) = input_range.intersection(range) {
            intersected = Some(Self {
                input_start: overlapping_range.start,
                output_start: self
                    .try_translate_point(overlapping_range.start)
                    .expect("I know it contains it"),
                length: overlapping_range.length,
            });
        }
        for diff_range in range.difference(&input_range) {
            output.push(Self::identity_map_from_range(&diff_range));
        }

        (intersected, output)
    }

    fn input_range(&self) -> Range {
        Range::from_magnitude(self.input_start, self.length)
    }

    fn output_range(&self) -> Range {
        Range::from_magnitude(self.output_start, self.length)
    }
}

impl std::fmt::Display for MapRange {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "{} -> {}", self.input_range(), self.output_range())
    }
}

struct TranslatorMap {
    maps: Vec<MapRange>,
}

impl TranslatorMap {
    fn from_lines(mut lines: RawLines) -> Self {
        let sub_map_lines = lines.split_off(1);
        let mut sub_maps = sub_map_lines
            .iter()
            .map(|line| MapRange::from_line(line))
            .collect::<Vec<MapRange>>();
        sub_maps.sort_by_key(|elem| elem.input_start);
        TranslatorMap { maps: sub_maps }
    }

    fn translate(&self, input: Element) -> Element {
        for sub_map in &self.maps {
            if let Some(output) = sub_map.try_translate_point(input) {
                return output;
            }
        }
        input
    }

    fn translate_range(&self, range: &Range) -> Vec<Range> {
        let mut input = vec![range.clone()];
        let mut output = vec![];

        for sub_map in &self.maps {
            let mut untranslated = vec![];
            let mut translated = vec![];
            for input_range in &input {
                let (intersected, nonintersected) = sub_map.translate_range(input_range);
                if let Some(intersected) = intersected {
                    translated.push(intersected.output_range());
                }
                untranslated.extend(nonintersected.into_iter().map(|range| range.output_range()));
            }
            output.append(&mut translated);
            input = untranslated;
        }
        output.append(&mut input);

        Range::union_all(output)
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
    for line in data {
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
    let mut seed_ranges = seeds.clone();
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

    let mut ranges = vec![];
    while let Some(length) = seed_ranges.pop() {
        let range_start = seed_ranges.pop().expect("Pairs");
        ranges.push(Range::from_magnitude(range_start, length));
    }

    ranges = Range::union_all(ranges);

    let mut lowest = Element::MAX;
    for range in ranges {
        let mut output = vec![range.clone()];
        for map in &maps {
            let mut intermediate = vec![];
            for input_range in output {
                let mut translated = map.translate_range(&input_range);
                intermediate.append(&mut translated);
            }
            output = intermediate;
        }
        let local_lowest = output
            .iter()
            .map(|element| element.start)
            .min()
            .expect("Something here");
        lowest = Element::min(lowest, local_lowest);
    }

    println!("P2 location: {lowest}");

    Ok(())
}

#[cfg(test)]
mod test {

    use crate::Range;

    #[test]
    fn test_contains() {
        let a = Range::from_magnitude(3, 5);
        let b = Range::from_magnitude(4, 2);
        let c = Range::from_magnitude(10, 4);

        assert!(a.contains(&b));
        assert!(!b.contains(&a));
        assert!(!c.contains(&a));
        assert!(!c.contains(&b));
    }

    #[test]
    fn test_intersect() {
        let a = Range::from_magnitude(3, 5);
        let b = Range::from_magnitude(4, 2);
        let c = Range::from_magnitude(10, 4);
        let d = Range::from_magnitude(11, 9);

        assert_eq!(a.intersection(&c), None);
        assert_eq!(a.intersection(&b), Some(b.clone()));
        assert_eq!(
            c.intersection(&d),
            Some(Range::from_exclusive_range(d.start, c.exclusive_end()))
        );
    }

    #[test]
    fn test_union_all() {
        let a = Range::from_magnitude(3, 5);
        // println!("a: {a}");
        let b = Range::from_magnitude(4, 2);
        // println!("b: {b}");
        let c = Range::from_magnitude(10, 4);
        // println!("c: {c}");
        let d = Range::from_magnitude(11, 9);
        // println!("d: {d}");

        let ranges = vec![a, b, c, d];

        let union = Range::union_all(ranges);
        assert_eq!(
            union,
            vec![Range::from_magnitude(3, 5), Range::from_magnitude(10, 10)]
        );
    }

    #[test]
    fn test_difference() {
        let a = Range::from_inclusive_range(57, 69);
        let b = Range::from_inclusive_range(53, 60);

        assert_eq!(a.difference(&b), vec![Range::from_inclusive_range(61, 69)]);
    }
}

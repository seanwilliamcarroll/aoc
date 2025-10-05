use std::collections::HashMap;
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

type Bid = usize;

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone)]
enum Card {
    Joker,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King,
    Ace,
}

impl Card {
    fn from_char(character: char) -> Self {
        match character {
            'A' => Self::Ace,
            'K' => Self::King,
            'Q' => Self::Queen,
            'J' => Self::Jack,
            'T' => Self::Ten,
            '9' => Self::Nine,
            '8' => Self::Eight,
            '7' => Self::Seven,
            '6' => Self::Six,
            '5' => Self::Five,
            '4' => Self::Four,
            '3' => Self::Three,
            '2' => Self::Two,
            _ => panic!("Invalid card!"),
        }
    }

    fn to_joker_rules(self) -> Self {
        match self {
            Self::Jack => Self::Joker,
            other => other.clone(),
        }
    }
}

type Hand = [Card; 5];

fn make_hand(line: &str) -> Hand {
    assert!(line.len() == 5);
    [
        Card::from_char(line.chars().nth(0).expect("Bad formatting")),
        Card::from_char(line.chars().nth(1).expect("Bad formatting")),
        Card::from_char(line.chars().nth(2).expect("Bad formatting")),
        Card::from_char(line.chars().nth(3).expect("Bad formatting")),
        Card::from_char(line.chars().nth(4).expect("Bad formatting")),
    ]
}

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord)]
enum Scores {
    HighCard,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    FullHouse,
    FourOfAKind,
    FiveOfAKind,
}

impl Scores {
    fn from_unique_cards_map(unique_cards_map: &HashMap<Card, usize>) -> Self {
        let unique_cards = unique_cards_map_to_unique_cards_vec(unique_cards_map);
        match unique_cards[..] {
            [_, _, _, _, _] => Self::HighCard,
            [_, _, _, _] => Self::OnePair,
            [(_, first), (_, middle), (_, last)] => {
                if first == 1 && middle == 1 && last == 3 {
                    Self::ThreeOfAKind
                } else {
                    // 1, 2, 2
                    Self::TwoPair
                }
            }
            [(_, first), (_, last)] => {
                if first == 1 && last == 4 {
                    Self::FourOfAKind
                } else {
                    // 2, 3
                    Self::FullHouse
                }
            }
            [_] => Self::FiveOfAKind,
            _ => panic!("Not possible"),
        }
    }

    fn from_hand(hand: &Hand) -> Self {
        let mut unique_cards_map = hand_to_unique_cards_map(hand);
        if let Some(joker_count) = unique_cards_map.remove(&Card::Joker) {
            let unique_cards = unique_cards_map_to_unique_cards_vec(&unique_cards_map);
            match joker_count {
                1 => match unique_cards[..] {
                    [_, _, _, _] => Self::OnePair,
                    [_, _, _] => Self::ThreeOfAKind,
                    [(_, 1), (_, 3)] => Self::FourOfAKind,
                    [(_, 2), (_, 2)] => Self::FullHouse,
                    [_] => Self::FiveOfAKind,
                    _ => panic!("Not possible"),
                },
                2 => match unique_cards[..] {
                    [_, _, _] => Self::ThreeOfAKind,
                    [_, _] => Self::FourOfAKind,
                    [_] => Self::FiveOfAKind,
                    _ => panic!("Not possible"),
                },
                3 => match unique_cards[..] {
                    [(_, 1), (_, 1)] => Self::FourOfAKind,
                    [(_, 2)] => Self::FiveOfAKind,
                    _ => panic!("Not possible"),
                },
                4 => Self::FiveOfAKind,
                5 => Self::FiveOfAKind,
                _ => panic!("Not possible!"),
            }
        } else {
            Self::from_unique_cards_map(&unique_cards_map)
        }
    }
}

fn hand_is_greater(hand_a: &Hand, hand_b: &Hand) -> bool {
    // Returns true if hand_a > hand_b
    let score_a = Scores::from_hand(hand_a);
    let score_b = Scores::from_hand(hand_b);
    if score_a > score_b {
        true
    } else if score_a < score_b {
        false
    } else {
        let card_pairs: [(&Card, &Card); 5] = hand_a
            .iter()
            .zip(hand_b.iter())
            .collect::<Vec<(&Card, &Card)>>()
            .try_into()
            .expect("Must be 5");
        for (card_a, card_b) in card_pairs {
            if card_a == card_b {
                continue;
            }
            return card_a > card_b;
        }
        panic!("Equal hands?");
    }
}

fn hand_to_unique_cards_map(hand: &Hand) -> HashMap<Card, usize> {
    let mut unique_cards = HashMap::new();

    for card in hand {
        *unique_cards.entry(card.clone()).or_insert(0) += 1;
    }
    unique_cards
}

fn unique_cards_map_to_unique_cards_vec(unique_cards: &HashMap<Card, usize>) -> Vec<(Card, usize)> {
    let mut unique_cards = unique_cards
        .iter()
        .map(|(card, count)| (card.clone(), *count))
        .collect::<Vec<(Card, usize)>>();
    unique_cards.sort_by_key(|(_, count)| *count);
    unique_cards
}

struct HandBid {
    hand: Hand,
    bid: Bid,
}

impl Ord for HandBid {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        let hand_a = &self.hand;
        let hand_b = &other.hand;
        let score_a = Scores::from_hand(hand_a);
        let score_b = Scores::from_hand(hand_b);
        if score_a == score_b {
            if hand_is_greater(hand_a, hand_b) {
                std::cmp::Ordering::Greater
            } else {
                std::cmp::Ordering::Less
            }
        } else {
            score_a.cmp(&score_b)
        }
    }
}

impl PartialOrd for HandBid {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

impl PartialEq for HandBid {
    fn eq(&self, other: &Self) -> bool {
        self.hand == other.hand
    }
}

impl Eq for HandBid {}

impl HandBid {
    fn from_line(line: &str) -> Self {
        let (hand, bid) = line.split_once(' ').expect("Bad formatting");
        let bid = bid.parse::<Bid>().expect("Bad Formatting");
        Self {
            hand: make_hand(hand),
            bid,
        }
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;
    println!("Found {} hands", raw_lines.len());

    let mut hand_bids = raw_lines
        .iter()
        .map(|line| HandBid::from_line(&line))
        .collect::<Vec<HandBid>>();

    hand_bids.sort();

    let mut sum_p1 = 0;
    for (index, hand_bid) in hand_bids.iter().enumerate() {
        let index = index + 1usize;
        sum_p1 += index * hand_bid.bid;
    }

    println!("P1 sum: {sum_p1}");

    let mut sum_p2 = 0;
    hand_bids = hand_bids
        .into_iter()
        .map(|HandBid { hand, bid }| HandBid {
            hand: hand
                .into_iter()
                .map(Card::to_joker_rules)
                .collect::<Vec<Card>>()
                .try_into()
                .expect("Must be 5"),
            bid,
        })
        .collect::<Vec<HandBid>>();
    hand_bids.sort();
    for (index, hand_bid) in hand_bids.iter().enumerate() {
        let index = index + 1usize;
        sum_p2 += index * hand_bid.bid;
    }

    println!("P2 sum: {sum_p2}");

    Ok(())
}

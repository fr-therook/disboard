use sac::Position;

#[cxx::bridge(namespace = "librustdisboard")]
mod ffi {
    pub struct Uuid {
        pub l: u32,
        pub w1: u16,
        pub w2: u16,
        pub b: [u8; 8],
    }

    pub enum Color {
        Black = 0,
        White = 1,
    }

    pub enum Role {
        Pawn = 1,
        Knight = 2,
        Bishop = 3,
        Rook = 4,
        Queen = 5,
        King = 6,
    }

    pub struct Piece {
        pub color: Color,
        pub role: Role,
    }

    extern "Rust" {
        fn piece_default() -> Piece;
    }

    #[derive(PartialEq)]
    pub struct Square {
        pub index: u8,
    }

    extern "Rust" {
        fn square_default() -> Square;
        fn square_from_coords(file: u8, rank: u8) -> Square;
        fn file(self: &Square) -> u8;
        fn rank(self: &Square) -> u8;
    }

    extern "Rust" {
        type Move;
        fn clone(&self) -> Box<Move>;
        fn from(&self) -> Square;
        fn to(&self) -> Square;
        fn is_promotion(&self) -> bool;
        fn set_promotion(&mut self, role: Role);
        fn is_en_passant(&self) -> bool;
        fn is_castle(&self) -> bool;
        fn castle_rook_from(&self) -> Square;
        fn castle_rook_to(&self) -> Square;
        fn to_string(&self) -> String;
    }

    extern "Rust" {
        type CurPosition;
        fn turn(&self) -> Color;

        fn squares(&self) -> Vec<Square>;
        fn pieces(&self) -> Vec<Piece>;

        fn has_piece_at(&self, square: Square) -> bool;
        fn piece_at(&self, square: Square) -> Piece;
        fn has_legal_move(&self, src: Square, dest: Square) -> bool;
        fn legal_move(&self, src: Square, dest: Square) -> Box<Move>;

        fn hints(&self, src: Square) -> Vec<Square>;
        fn captures(&self, src: Square) -> Vec<Square>;
    }

    extern "Rust" {
        type GameTree;
        fn game_default() -> Box<GameTree>;

        fn root(&self) -> Uuid;
        fn position(&self, node: Uuid) -> Box<CurPosition>;

        fn has_prev_move(&self, node: Uuid) -> bool;
        fn prev_move(&self, node: Uuid) -> Box<Move>;

        fn has_prev_node(&self, node: Uuid) -> bool;
        fn prev_node(&self, node: Uuid) -> Uuid;
        fn has_next_mainline_node(&self, node: Uuid) -> bool;
        fn next_mainline_node(&self, node: Uuid) -> Uuid;

        fn variations(&self, node: Uuid) -> Vec<Uuid>;
        fn siblings(&self, node: Uuid) -> Vec<Uuid>;
        fn mainline_nodes(&self, node: Uuid) -> Vec<Uuid>;

        fn add_node(&mut self, node: Uuid, m: Box<Move>) -> Uuid;

        fn pgn(&self) -> String;
    }
}

impl Into<ffi::Uuid> for uuid::Uuid {
    fn into(self) -> ffi::Uuid {
        ffi::Uuid {
            l: self.as_fields().0,
            w1: self.as_fields().1,
            w2: self.as_fields().2,
            b: *self.as_fields().3,
        }
    }
}

impl Into<uuid::Uuid> for ffi::Uuid {
    fn into(self) -> uuid::Uuid {
        uuid::Uuid::from_fields(self.l, self.w1, self.w2, &self.b)
    }
}

macro_rules! convert_enum {
    ($src: ty, $dst: ty, $($variant: ident,)+) => {
        impl Into<$dst> for $src {
            fn into(self) -> $dst {
                match self {
                    $(Self::$variant => <$dst>::$variant,)*
                    _ => panic!(),
                }
            }
        }
    }
}

convert_enum!(sac::Color, ffi::Color, Black, White,);

convert_enum!(
    sac::Role,
    ffi::Role,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
);

convert_enum!(
    ffi::Role,
    sac::Role,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
);

impl Into<ffi::Piece> for sac::Piece {
    fn into(self) -> ffi::Piece {
        ffi::Piece {
            color: self.color.into(),
            role: self.role.into(),
        }
    }
}

fn piece_default() -> ffi::Piece {
    ffi::Piece {
        color: ffi::Color::Black,
        role: ffi::Role::Knight,
    }
}

impl Into<ffi::Square> for sac::Square {
    fn into(self) -> ffi::Square {
        ffi::Square {
            index: u8::from(self),
        }
    }
}

impl Into<sac::Square> for ffi::Square {
    fn into(self) -> sac::Square {
        sac::Square::new(self.index as u32)
    }
}

fn square_default() -> ffi::Square {
    ffi::Square { index: 0 }
}

fn square_from_coords(file: u8, rank: u8) -> ffi::Square {
    let sq = sac::Square::from_coords(sac::File::new(file as u32), sac::Rank::new(rank as u32));
    ffi::Square {
        index: u8::from(sq),
    }
}

impl ffi::Square {
    fn file(&self) -> u8 {
        self.index & 7
    }
    fn rank(&self) -> u8 {
        self.index >> 3
    }
}

struct Move {
    inner: sac::Move,
    san: sac::SanPlus,
}

impl Move {
    fn clone(&self) -> Box<Move> {
        Box::new(Move {
            inner: self.inner.clone(),
            san: self.san.clone(),
        })
    }

    fn from(&self) -> ffi::Square {
        self.inner
            .from()
            .expect("a chess move always comes from somewhere")
            .into()
    }

    fn to(&self) -> ffi::Square {
        if let sac::Move::Castle { king: _, rook: _ } = self.inner {
            // Treat castling as special case
            let castling_side = self.inner.castling_side().unwrap();

            let to_rank = self.inner.from().unwrap().rank();
            let to_file = castling_side.king_to_file();

            let dest_sq = sac::Square::from_coords(to_file, to_rank);
            return dest_sq.into();
        }

        self.inner.to().into()
    }

    fn is_promotion(&self) -> bool {
        self.inner.is_promotion()
    }

    fn set_promotion(&mut self, role: ffi::Role) {
        if let sac::Move::Normal {
            ref mut promotion, ..
        } = self.inner
        {
            *promotion = Some(role.into());
        }
    }

    fn is_en_passant(&self) -> bool {
        self.inner.is_en_passant()
    }

    fn is_castle(&self) -> bool {
        self.inner.is_castle()
    }

    fn castle_rook_from(&self) -> ffi::Square {
        if let sac::Move::Castle { king: _, rook } = self.inner {
            return rook.into();
        }

        ffi::Square { index: 0 }
    }

    fn castle_rook_to(&self) -> ffi::Square {
        if self.inner.is_castle() {
            let castling_side = self.inner.castling_side().unwrap();
            let from_rank = self.inner.from().unwrap().rank();
            let to_file = castling_side.rook_to_file();

            return sac::Square::from_coords(to_file, from_rank).into();
        }

        ffi::Square { index: 0 }
    }

    fn to_string(&self) -> String {
        format!("{}", self.san)
    }
}

struct CurPosition(sac::Chess);

impl CurPosition {
    fn turn(&self) -> ffi::Color {
        self.0.turn().into()
    }

    fn squares(&self) -> Vec<ffi::Square> {
        let board = self.0.board().clone();

        board
            .into_iter()
            .map(|(sq, _)| sq.into())
            .collect::<Vec<ffi::Square>>()
    }

    fn pieces(&self) -> Vec<ffi::Piece> {
        let board = self.0.board().clone();

        board
            .into_iter()
            .map(|(_, p)| p.into())
            .collect::<Vec<ffi::Piece>>()
    }

    fn has_piece_at(&self, square: ffi::Square) -> bool {
        let square: sac::Square = square.into();
        self.0.board().piece_at(square).is_some()
    }

    fn piece_at(&self, square: ffi::Square) -> ffi::Piece {
        let square: sac::Square = square.into();
        self.0
            .board()
            .piece_at(square)
            .map(|val| val.into())
            .unwrap_or(piece_default())
    }

    fn has_legal_move(&self, src: ffi::Square, dest: ffi::Square) -> bool {
        self._legal_move(src, dest).is_some()
    }

    fn legal_move(&self, src: ffi::Square, dest: ffi::Square) -> Box<Move> {
        let legal_move = self._legal_move(src, dest).unwrap_or(sac::Move::Put {
            role: sac::Role::Pawn,
            to: sac::Square::A1,
        });
        let san = sac::SanPlus::from_move(self.0.clone(), &legal_move);
        Box::new(Move {
            inner: legal_move,
            san,
        })
    }

    fn hints(&self, src: ffi::Square) -> Vec<ffi::Square> {
        let (hint_vec, _) = self._legal_moves(src);
        hint_vec
    }

    fn captures(&self, src: ffi::Square) -> Vec<ffi::Square> {
        let (_, captures_vec) = self._legal_moves(src);
        captures_vec
    }
}

impl CurPosition {
    fn _legal_move(&self, src_sq: ffi::Square, dest_sq: ffi::Square) -> Option<sac::Move> {
        let src_sq: sac::Square = src_sq.into();
        let dest_sq: sac::Square = dest_sq.into();

        let move_vec: Vec<sac::Move> = self
            .0
            .legal_moves()
            .into_iter()
            .filter(|v| v.from().unwrap() == src_sq)
            .collect::<Vec<sac::Move>>();

        for m in move_vec {
            if let sac::Move::Castle { king, rook } = m {
                let castling_side = m.castling_side().unwrap();
                let to_file = castling_side.king_to_file();
                let to_king_sq = sac::Square::from_coords(to_file, king.rank());
                if to_king_sq == dest_sq {
                    return Some(m);
                }
                if rook == dest_sq {
                    return Some(m);
                }
                continue;
            }

            if m.to() == dest_sq {
                // A legal move!
                return Some(m);
            }
        }

        None
    }

    fn _legal_moves(&self, sq: ffi::Square) -> (Vec<ffi::Square>, Vec<ffi::Square>) {
        let sq: sac::Square = sq.into();
        let mut move_vec = self
            .0
            .legal_moves()
            .into_iter()
            .filter(|v| v.from().unwrap() == sq)
            .collect::<Vec<sac::Move>>();
        move_vec.dedup_by(|l, r| {
            if !l.is_promotion() || !r.is_promotion() {
                return false;
            }

            l.to() == r.to()
        });

        let (castling_move_vec, move_vec): (Vec<sac::Move>, Vec<sac::Move>) =
            move_vec.into_iter().partition(|m| m.is_castle());

        let (capture_vec, move_vec): (Vec<sac::Move>, Vec<sac::Move>) =
            move_vec.into_iter().partition(|m| m.capture().is_some());
        let mut move_vec = move_vec
            .into_iter()
            .map(|m| m.to().into())
            .collect::<Vec<ffi::Square>>();
        let mut capture_vec = capture_vec
            .into_iter()
            .map(|m| m.to().into())
            .collect::<Vec<ffi::Square>>();

        for castling_move in castling_move_vec {
            if let sac::Move::Castle { king: _, rook } = castling_move {
                let castling_side = castling_move.castling_side().unwrap();

                let to_rank = castling_move.from().unwrap().rank();
                let to_file = castling_side.king_to_file();

                move_vec.push(sac::Square::from_coords(to_file, to_rank).into());

                capture_vec.push(rook.into());
            }
        }

        (move_vec, capture_vec)
    }
}

struct GameTree {
    inner: sac::Game,
}

fn game_default() -> Box<GameTree> {
    Box::new(GameTree {
        inner: sac::Game::default(),
    })
}

impl GameTree {
    fn root(&self) -> ffi::Uuid {
        self.inner.root().into()
    }

    fn position(&self, node: ffi::Uuid) -> Box<CurPosition> {
        Box::new(CurPosition(
            self.inner.board_at(node.into()).expect("invalid node"),
        ))
    }

    fn has_prev_move(&self, node: ffi::Uuid) -> bool {
        self.inner.prev_move(node.into()).is_some()
    }

    fn prev_move(&self, node: ffi::Uuid) -> Box<Move> {
        let node = node.into();
        let m = self.inner.prev_move(node).unwrap_or(sac::Move::Put {
            role: sac::Role::Pawn,
            to: sac::Square::A1,
        });
        let pos = self.inner.board_before(node).expect("invalid node");
        let san = sac::SanPlus::from_move(pos, &m);

        Box::new(Move { inner: m, san })
    }

    fn has_prev_node(&self, node: ffi::Uuid) -> bool {
        self.inner.parent(node.into()).is_some()
    }
    fn prev_node(&self, node: ffi::Uuid) -> ffi::Uuid {
        self.inner.parent(node.into()).unwrap_or_default().into()
    }
    fn has_next_mainline_node(&self, node: ffi::Uuid) -> bool {
        self.inner.mainline(node.into()).is_some()
    }
    fn next_mainline_node(&self, node: ffi::Uuid) -> ffi::Uuid {
        self.inner.mainline(node.into()).unwrap_or_default().into()
    }

    fn variations(&self, node: ffi::Uuid) -> Vec<ffi::Uuid> {
        let node: uuid::Uuid = node.into();
        let variation_vec = self.inner.other_variations(node);
        println!("variations for {:?}: {:?}", node, variation_vec);

        variation_vec
            .into_iter()
            .map(|val| val.into())
            .collect::<Vec<ffi::Uuid>>()
    }

    fn siblings(&self, node: ffi::Uuid) -> Vec<ffi::Uuid> {
        let node: uuid::Uuid = node.into();
        let sibling_vec = self.inner.siblings(node);

        sibling_vec
            .into_iter()
            .map(|val| val.into())
            .collect::<Vec<ffi::Uuid>>()
    }

    fn mainline_nodes(&self, node: ffi::Uuid) -> Vec<ffi::Uuid> {
        let mut cur_node: uuid::Uuid = node.into();
        let mut node_vec: Vec<uuid::Uuid> = Vec::new();
        while let Some(node) = self.inner.mainline(cur_node) {
            node_vec.push(node);
            cur_node = node;
        }

        node_vec
            .into_iter()
            .map(|val| val.into())
            .collect::<Vec<ffi::Uuid>>()
    }

    fn add_node(&mut self, node: ffi::Uuid, m: Box<Move>) -> ffi::Uuid {
        self.inner
            .add_node(node.into(), m.inner)
            .expect("invalid node in add_node")
            .into()
    }

    fn pgn(&self) -> String {
        format!("{}", self.inner)
    }
}

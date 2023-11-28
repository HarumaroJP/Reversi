#include "../include/Board.h"

namespace Reversi
{
	Board::Board() :
		black_board(0),
		white_board(0)
	{
		Reset();
	}

	void Board::Reset()
	{
		black_board = 0b0000000000000000000000000000100000010000000000000000000000000000ull;
		white_board = 0b0000000000000000000000000001000000001000000000000000000000000000ull;
	}

	void Board::Set(const u64 input, const Side side)
	{
		u64& side_data = (side == Side::Black ? black_board : white_board);
		side_data |= input;
	}

	u64 Board::Flip(const u64 input, const Side side)
	{
		u64& mine = side == Side::Black ? black_board : white_board;
		u64& others = side == Side::Black ? white_board : black_board;
		u64 flips = GetHorizontalFlips(input, mine, others) |
			GetVerticalFlips(input, mine, others) |
			GetDiagonalCrossFlips(input, mine, others);

		mine |= flips;
		others ^= flips;

		return flips;
	}

	void Board::Undo(const u64 input, const Side side)
	{
		u64& origin_side = (side == Side::Black ? black_board : white_board);
		u64& setter_side = (side == Side::Black ? white_board : black_board);

		origin_side &= ~input;
		setter_side |= input;
	}

	void Board::SetEmpty(const u64 input)
	{
		black_board &= ~input;
		white_board &= ~input;
	}

	std::pair<int, int> Board::CountStone() const
	{
		return std::make_pair(std::popcount(black_board), std::popcount(white_board));
	}

	std::pair<int, int> Board::CountLegalMoves() const
	{
		return std::make_pair(std::popcount(GetLegalMoves(Side::Black)), std::popcount(GetLegalMoves(Side::White)));
	}

	u64 Board::GetLegalMoves(const Side side) const
	{
		u64 mine = side == Side::Black ? black_board : white_board;
		u64 others = side == Side::Black ? white_board : black_board;
		u64 empties = ~GetAllBoard();

		return GetVerticalMoves(mine, others, empties) |
			GetHorizontalMoves(mine, others, empties) |
			GetCrossMoves(mine, others, empties);
	}

	std::pair<u64, u64> Board::GetFieldData() const
	{
		return std::make_pair(black_board, white_board);
	}

	u64 Board::GetAllBoard() const
	{
		return black_board | white_board;
	}

	u64 Board::GetVerticalMoves(const u64 mine, const u64 others, const u64 empties) const
	{
		//上下端のマスを除く
		u64 vertical_cells = others & vertical_mask;

		return GetShiftedMoves(mine, vertical_cells, empties, SHIFT_VERTICAL);
	}

	u64 Board::GetHorizontalMoves(const u64 mine, const u64 others, const u64 empties) const
	{
		//左右端のマスを除く
		u64 horizontal_cells = others & horizontal_mask;

		return GetShiftedMoves(mine, horizontal_cells, empties, SHIFT_HORIZONTAL);
	}

	u64 Board::GetCrossMoves(const u64 mine, const u64 others, const u64 empties) const
	{
		//すべての端のマスを除く
		u64 cross_cells = others & allSide_mask;

		return GetShiftedMoves(mine, cross_cells, empties, SHIFT_VERTICAL + SHIFT_HORIZONTAL) |
			GetShiftedMoves(mine, cross_cells, empties, SHIFT_VERTICAL - SHIFT_HORIZONTAL);
	}

	u64 Board::GetHorizontalFlips(const u64 input, const u64 mine, const u64 others) const
	{
		u64 horizontal_cells = others & horizontal_mask;

		return GetShiftedFlips(input, mine, horizontal_cells, SHIFT_HORIZONTAL);
	}

	u64 Board::GetVerticalFlips(const u64 input, const u64 mine, const u64 others) const
	{
		u64 vertical_cells = others & vertical_mask;

		return GetShiftedFlips(input, mine, vertical_cells, SHIFT_VERTICAL);
	}

	u64 Board::GetDiagonalCrossFlips(const u64 input, const u64 mine, const u64 others) const
	{
		u64 cross_cells = others & allSide_mask;

		return GetShiftedFlips(input, mine, cross_cells, SHIFT_VERTICAL - SHIFT_HORIZONTAL) |
			GetShiftedFlips(input, mine, cross_cells, SHIFT_VERTICAL + SHIFT_HORIZONTAL);
	}

	u64 Board::GetShiftedMoves(const u64 mine, const u64 cells, const u64 empties, const int shift) const
	{
		u64 moves;

		//シフトして配置可能マスを絞る
		//高速化のためコンパイラ側で展開してもいいが、コードが複雑になるので手動で展開
		u64 tmp = cells & (mine << shift);
		tmp |= cells & (tmp << shift);
		tmp |= cells & (tmp << shift);
		tmp |= cells & (tmp << shift);
		tmp |= cells & (tmp << shift);
		tmp |= cells & (tmp << shift);
		moves = empties & (tmp << shift);

		tmp = cells & (mine >> shift);
		tmp |= cells & (tmp >> shift);
		tmp |= cells & (tmp >> shift);
		tmp |= cells & (tmp >> shift);
		tmp |= cells & (tmp >> shift);
		tmp |= cells & (tmp >> shift);
		moves |= empties & (tmp >> shift);

		return moves;
	}

	u64 Board::GetShiftedFlips(const u64 input, const u64 mine, const u64 cells, const int shift) const
	{
		u64 flips = 0ull;

		u64 flip = cells & (input >> shift);
		flip |= cells & (flip >> shift);
		flip |= cells & (flip >> shift);
		flip |= cells & (flip >> shift);
		flip |= cells & (flip >> shift);
		flip |= cells & (flip >> shift);
		flip |= cells & (flip >> shift);
		flips = flip & -(long long)((mine & (flip >> shift)) != 0);

		flip = cells & (input << shift);
		flip |= cells & (flip << shift);
		flip |= cells & (flip << shift);
		flip |= cells & (flip << shift);
		flip |= cells & (flip << shift);
		flip |= cells & (flip << shift);
		flip |= cells & (flip << shift);
		flips |= flip & -(long long)((mine & (flip << shift)) != 0);

		return flips;
	}

	u64 Board::GetCrossFloods(const u64 input, const u64 others) const
	{
		u64 vertical_cells = others & vertical_mask;
		u64 horizontal_cells = others & horizontal_mask;
		u64 floods;

		//上
		u64 flood = vertical_cells & (input << SHIFT_VERTICAL);
		flood |= vertical_cells & (flood << SHIFT_VERTICAL);
		flood |= vertical_cells & (flood << SHIFT_VERTICAL);
		flood |= vertical_cells & (flood << SHIFT_VERTICAL);
		flood |= vertical_cells & (flood << SHIFT_VERTICAL);
		flood |= vertical_cells & (flood << SHIFT_VERTICAL);
		flood |= vertical_cells & (flood << SHIFT_VERTICAL);
		floods = flood;

		//下
		flood = vertical_cells & (input >> SHIFT_VERTICAL);
		flood |= vertical_cells & (flood >> SHIFT_VERTICAL);
		flood |= vertical_cells & (flood >> SHIFT_VERTICAL);
		flood |= vertical_cells & (flood >> SHIFT_VERTICAL);
		flood |= vertical_cells & (flood >> SHIFT_VERTICAL);
		flood |= vertical_cells & (flood >> SHIFT_VERTICAL);
		flood |= vertical_cells & (flood >> SHIFT_VERTICAL);
		floods |= flood;

		//右
		flood = horizontal_cells & (input >> SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood >> SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood >> SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood >> SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood >> SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood >> SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood >> SHIFT_HORIZONTAL);
		floods = flood;

		//左
		flood = horizontal_cells & (input << SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood << SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood << SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood << SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood << SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood << SHIFT_HORIZONTAL);
		flood |= horizontal_cells & (flood << SHIFT_HORIZONTAL);
		floods |= flood;

		return floods;
	}

	void Board::Overwrite(const Board& board)
	{
		black_board = board.black_board;
		white_board = board.white_board;
	}
}

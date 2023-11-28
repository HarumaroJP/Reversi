#pragma once

#include "Basic.h"
#include <intrin.h>
#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <memory>
#include <span>

namespace Reversi
{

	/// <summary>
	/// 盤面を管理するクラス
	/// </summary>
	class Board
	{
	public:
		Board();

		/// <summary>
		/// 盤面に石をセットします
		/// </summary>
		/// <param name="input">セット位置</param>
		/// <param name="side">セットする側</param>
		void Set(u64 input, Side side);

		/// <summary>
		/// 指定した位置の石を反転し、反転位置を取得します。
		/// </summary>
		/// <param name="input">設置位置</param>
		/// <param name="side">反転する側</param>
		/// <returns>反転位置</returns>
		u64 Flip(u64 input, Side side);

		/// <summary>
		/// 指定した位置の石情報を巻き戻します
		/// </summary>
		/// <param name="input">巻き戻す位置</param>
		/// <param name="side">セットした側</param>
		void Undo(u64 input, Side side);

		/// <summary>
		/// 指定した場所を空にします
		/// </summary>
		/// <param name="input">空にする位置</param>
		void SetEmpty(u64 input);

		/// <summary>
		/// 石の数を取得します
		/// </summary>
		/// <returns>黒番と白番の石の数</returns>
		std::pair<int, int> CountStone() const;

		/// <summary>
		/// 着手可能数を取得します
		/// </summary>
		/// <returns>黒番と白番の着手可能数</returns>
		std::pair<int, int> CountLegalMoves() const;

		/// <summary>
		/// 石が置かれている位置を取得します
		/// </summary>
		/// <returns>石が置かれている位置</returns>
		u64 GetAllBoard() const;

		/// <summary>
		/// 着手可能位置を取得します
		/// </summary>
		/// <param name="side">着手可能側</param>
		/// <returns>着手可能位置</returns>
		u64 GetLegalMoves(Side side) const;

		/// <summary>
		/// 指定した位置から十字に繋がったマスを取得します
		/// </summary>
		/// <param name="input">基準位置</param>
		/// <param name="others">取得する側</param>
		/// <returns>繋がったマスの情報</returns>
		u64 GetCrossFloods(u64 input, u64 others) const;

		/// <summary>
		/// 盤面情報を取得します
		/// </summary>
		/// <returns>黒番と白番の盤面情報</returns>
		std::pair<u64, u64> GetFieldData() const;

		/// <summary>
		/// 盤面情報のリセットを行います
		/// </summary>
		void Reset();

		/// <summary>
		/// 盤面情報を上書きします
		/// </summary>
		/// <param name="board">上書きする情報</param>
		void Overwrite(const Board& board);

	private:
		u64 black_board;
		u64 white_board;

		// ビット演算に使用する定数
		static constexpr int SHIFT_VERTICAL = 8;
		static constexpr int SHIFT_HORIZONTAL = 1;

		static constexpr u64 horizontal_mask = 0x7e7e7e7e7e7e7e7e;
		static constexpr u64 vertical_mask = 0x00FFFFFFFFFFFF00;
		static constexpr u64 allSide_mask = horizontal_mask & vertical_mask;

		u64 GetVerticalMoves(u64 mine, u64 others, u64 empties) const;
		u64 GetHorizontalMoves(u64 mine, u64 others, u64 empties) const;
		u64 GetCrossMoves(u64 mine, u64 others, u64 empties) const;

		u64 GetVerticalFlips(u64 input, u64 mine, u64 others) const;
		u64 GetHorizontalFlips(u64 input, u64 mine, u64 others) const;
		u64 GetDiagonalCrossFlips(u64 input, u64 mine, u64 others) const;

		u64 GetShiftedMoves(const u64 mine, const u64 others, const u64 empties, const int shift) const;
		u64 GetShiftedFlips(const u64 input, const u64 mine, const u64 others, const int shift) const;
	};
}
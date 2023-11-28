#pragma once

#include <string>
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <Windows.h>

#include "Basic.h"

namespace Reversi
{
	/// <summary>
	/// 盤面を描画するクラス
	/// </summary>
	class BoardWriter
	{
	public:
		explicit BoardWriter(int size);
		~BoardWriter();

		/// <summary>
		/// 盤面をコンソールに書き込みます
		/// </summary>
		/// <param name="field_data">黒番と白版の盤面情報</param>
		/// <param name="legal_moves">着手可能マス</param>
		/// <param name="input">入力マス</param>
		void Write(std::pair<u64, u64> field_data, u64 legal_moves, u64 input = 0xFFFFFFFFFFFFFFFF);

	private:
		const int board_size;
		std::wstring write_buffer;
		std::wstring defaultFontName;

		//色情報を変更する文字列を取得する
		std::wstring GetBackColorCode(const int id) const;
		std::wstring GetFrontColorCode(const int id) const;

		//指定した位置の石情報を文字列に変換する
		std::wstring ToStone(const u64 black_data, const u64 white_data, const u64 legal_moves, const int offset, const u64 input) const;

		void WriteAlphabets();
		void WriteParts(const wchar_t& l_side, const wchar_t& m_side, const wchar_t& r_side);
	};
}

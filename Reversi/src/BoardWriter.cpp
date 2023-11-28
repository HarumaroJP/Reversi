#include "../include/BoardWriter.h"

namespace Reversi
{
	BoardWriter::BoardWriter(int board_size) : board_size(board_size)
	{
		//MSゴシックに強制
		CONSOLE_FONT_INFOEX font = { sizeof(font) };
		HANDLE hcout = GetStdHandle(STD_OUTPUT_HANDLE);
		GetCurrentConsoleFontEx(hcout, FALSE, &font);
		defaultFontName = font.FaceName;
		wcscpy_s(font.FaceName, L"MS Gothic");
		SetCurrentConsoleFontEx(hcout, FALSE, &font);

		//色付き出力対応
		HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD mode = 0;
		GetConsoleMode(stdHandle, &mode);
		SetConsoleMode(stdHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

		//unicode対応
		_setmode(_fileno(stdout), _O_U16TEXT);
	}

	BoardWriter::~BoardWriter()
	{
		//元のフォントに戻す
		CONSOLE_FONT_INFOEX font = { sizeof(font) };
		HANDLE hcout = GetStdHandle(STD_OUTPUT_HANDLE);
		GetCurrentConsoleFontEx(hcout, FALSE, &font);
		wcscpy_s(font.FaceName, defaultFontName.c_str());
		SetCurrentConsoleFontEx(hcout, FALSE, &font);
	}

	void BoardWriter::Write(const std::pair<u64, u64> field_data, const u64 legal_moves, const u64 input)
	{
		u64 black = field_data.first;
		u64 white = field_data.second;

		write_buffer += L'\n';
		write_buffer += GetBackColorCode(22);
		write_buffer += L"                                       ";
		write_buffer += GetBackColorCode(-1);
		write_buffer += L'\n';
		write_buffer += GetBackColorCode(22);
		write_buffer += L"   ";
		WriteAlphabets();
		write_buffer += GetBackColorCode(22);
		write_buffer += L"   ";
		write_buffer += GetFrontColorCode(0);
		WriteParts(L'┏', L'┳', L'┓');

		for (int i = 0; i < board_size; i++)
		{
			write_buffer += GetBackColorCode(22);
			write_buffer += GetFrontColorCode(-1);
			write_buffer += L' ';
			write_buffer += (wchar_t)(L'1' + i);

			write_buffer += L' ';
			write_buffer += GetFrontColorCode(0);
			write_buffer += L'┃';

			for (int j = 0; j < board_size; j++)
			{
				write_buffer += L' ';
				write_buffer += ToStone(black, white, legal_moves, i * 8 + j, input);
				write_buffer += L'┃';
			}

			write_buffer += L"   ";
			write_buffer += GetBackColorCode(-1);
			write_buffer += L'\n';

			if (i < board_size - 1)
			{
				write_buffer += GetBackColorCode(22);
				write_buffer += GetFrontColorCode(-1);
				write_buffer += L"   ";

				write_buffer += GetFrontColorCode(0);
				WriteParts(L'┣', L'╋', L'┫');
			}
		}

		write_buffer += GetBackColorCode(22);
		write_buffer += L"   ";
		WriteParts(L'┗', L'┻', L'┛');
		write_buffer += GetBackColorCode(22);
		write_buffer += L"                                       ";
		write_buffer += GetFrontColorCode(-1);
		write_buffer += GetBackColorCode(-1);
		write_buffer += L'\n';

		//ボード表示だけ高速なprintfを使う
		wprintf(write_buffer.c_str());

		write_buffer.clear();
	}

	std::wstring BoardWriter::ToStone(const u64 black_data, const u64 white_data, const u64 legal_moves, const int offset, const u64 input) const
	{
		std::wstring stone;

		bool is_input = input != 0xFFFFFFFFFFFFFFFF && ((1ull << offset) & input) == input;

		if (is_input)
		{
			stone += GetBackColorCode(1);
		}

		if (((legal_moves >> offset) & 1ull) == 1ull)
		{
			stone += L'＋';
		}
		else if (((black_data >> offset) & 1ull) == 1ull)
		{
			stone += L'●';
		}
		else if (((white_data >> offset) & 1ull) == 1ull)
		{
			stone += GetFrontColorCode(231);
			stone += L'●';
			stone += GetFrontColorCode(0);
		}
		else
		{
			stone += L"  ";
		}

		if (is_input)
		{
			stone += GetBackColorCode(22);
		}

		return stone;
	}

	void BoardWriter::WriteParts(const wchar_t& l_side, const wchar_t& m_side, const wchar_t& r_side)
	{
		write_buffer += l_side;

		for (int j = 0; j < board_size - 1; j++)
		{
			write_buffer += L"━━━";
			write_buffer += m_side;
		}

		write_buffer += L"━━━";
		write_buffer += r_side;
		write_buffer += L"   ";
		write_buffer += GetBackColorCode(-1);
		write_buffer += L'\n';
	}

	void BoardWriter::WriteAlphabets()
	{
		write_buffer += L"  ";

		for (int i = 0; i < board_size; ++i)
		{
			write_buffer += (wchar_t)(L'a' + i);
			write_buffer += L"   ";
		}

		write_buffer += L"  ";
		write_buffer += GetBackColorCode(-1);
		write_buffer += L'\n';
	}

	std::wstring BoardWriter::GetBackColorCode(const int id) const
	{
		if (id < 0)
			return L"\033[49m";

		std::wstring code;
		code += L"\033[48;5;";
		code += std::to_wstring(id);
		code += L'm';
		return code;
	}

	std::wstring BoardWriter::GetFrontColorCode(const int id) const
	{
		if (id < 0)
			return L"\033[39m";

		std::wstring code;
		code += L"\033[38;5;";
		code += std::to_wstring(id);
		code += L'm';
		return code;
	}

}
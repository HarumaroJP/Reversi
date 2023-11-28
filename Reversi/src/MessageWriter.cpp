#include "../include/MessageWriter.h"

namespace Reversi
{
	void MessageWriter::WriteWelcomeMessage()
	{
		write_buffer += L"=======================================\n";
		write_buffer += L"リバーシプログラムへようこそ！\n";
		write_buffer += L"このプログラムは, Windowsのコマンドプロンプトのみサポートしています。\n";
		write_buffer += L"Windows TerminalではUnicode対応が不十分なため、レイアウトが崩れる可能性があります。\n";
		write_buffer += L"=======================================\n\n";
		write_buffer += L"ゲームを開始するにはEnterキーを押してください...\n";

		WriteMessage();
		getchar();
	}

	void MessageWriter::WritePassMessage(const Side side)
	{
		std::wstring sideChar = side == Side::Black ? L"黒" : L"白";
		write_buffer += sideChar + L"は石を置くことができません!\n";
		write_buffer += L"パスします。\n";

		WriteMessage();

		constexpr double wait_time = 1.0;
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(wait_time * 1000)));
	}

	void MessageWriter::WriteTurnMessage(const Side player_turn, const int max_depth)
	{
		std::wstring player = L"プレイヤー ";
		std::wstring enemy = L"敵AI Lv" + std::to_wstring(max_depth);

		write_buffer += L"黒番: ";
		write_buffer += player_turn == Side::Black ? player : enemy;
		write_buffer += L"白番: ";
		write_buffer += player_turn == Side::Black ? enemy : player;
		write_buffer += L"\n\n";

		WriteMessage();
	}

	void MessageWriter::WriteResultMessage(const int black_count, const int white_count)
	{
		std::wstring result;

		if (black_count > white_count)
		{
			result = L"黒の勝ち";
		}
		else if (black_count < white_count)
		{
			result = L"白の勝ち";
		}
		else
		{
			result = L"引き分け";
		}

		write_buffer += L"ゲーム終了!\n";
		write_buffer += result + L"です!\n";
		write_buffer += L"黒の数は: " + std::to_wstring(black_count) + L", 白の数は: " + std::to_wstring(white_count) + L"\n";

		WriteMessage();
	}

	void MessageWriter::WriteSelectTurnMessage(const bool invalid)
	{
		if (invalid)
		{
			write_buffer += L"0 or 1を入力してください。\n";
		}

		write_buffer += L"先行と後攻を選んでください。(先行: 0, 後攻: 1)\n";
		write_buffer += L">> ";

		WriteMessage();
	}


	void MessageWriter::WriteSelectStrengthMessage(const bool invalid, const int min_strength, const int max_strength)
	{
		if (invalid)
		{
			write_buffer += (L"\n" + std::to_wstring(min_strength) + L" ~ " + std::to_wstring(max_strength) + L"で入力してください。\n");
		}

		write_buffer += L"\n" + std::to_wstring(min_strength) + L" ~ " + std::to_wstring(max_strength) + L"で敵の強さを決めてください(スペックによっては7以下がおすすめです！)。\n";
		write_buffer += L">> ";

		WriteMessage();
	}

	void MessageWriter::WriteRetryMessage(const bool invalid)
	{
		if (invalid)
		{
			write_buffer += L"y or nを入力してください。\n";
		}

		write_buffer += L"リトライしますか？ (はい: y, いいえ: n)\n";
		write_buffer += L">> ";

		WriteMessage();
	}

	void MessageWriter::WriteMessage()
	{
		std::wcout << write_buffer;
		write_buffer.clear();
	}

	void MessageWriter::WriteGameStartMessage()
	{
		write_buffer += L"\n=======================================\n";
		write_buffer += L"ゲーム開始！\n";
		write_buffer += L"=======================================\n";

		WriteMessage();
	}
}
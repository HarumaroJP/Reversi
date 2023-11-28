#pragma once

#include <string>
#include <iostream>
#include <thread>

#include "Basic.h"

namespace Reversi
{
	/// <summary>
	/// ユーザーにメッセージを表示するクラス
	/// </summary>
	class MessageWriter
	{
		std::wstring write_buffer;
		void WriteMessage();
	public:
		void WriteWelcomeMessage();
		void WriteRetryMessage(const bool invalid);
		void WriteSelectTurnMessage(const bool invalid);
		void WriteResultMessage(const int black_count, const int white_count);
		void WritePassMessage(const Side side);
		void WriteTurnMessage(const Side player_turn, const int max_depth);
		void WriteSelectStrengthMessage(const bool invalid, const int min_strength, const int max_strength);
		void WriteGameStartMessage();
	};
}




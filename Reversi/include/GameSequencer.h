#pragma once

#include <random>
#include <chrono>
#include <thread>

#include "Board.h"
#include "ReversiEngine.h"
#include "BoardWriter.h"
#include "InputReader.h"
#include "MessageWriter.h"
#include "ReversiBenchmark.h"

namespace Reversi
{
	/// <summary>
	/// 現在の盤面の戦況を表すクラス
	/// </summary>
	struct BoardInfo
	{
		int black_count;
		int white_count;
		bool is_end;
	};

	/// <summary>
	/// ゲームの進行を管理するクラス
	/// </summary>
	class GameSequencer
	{
	public:
		GameSequencer(std::shared_ptr<Board>& board, std::shared_ptr<BoardWriter>& board_writer, std::shared_ptr<MessageWriter>& message_writer);
	
		void Start();
	
	private:
		// 敵AIの強さ（探索の深さ）の定数
		// マジックナンバーを定数化（または外部データとして所持）することで
		// 容易に変更が可能になります。マジックナンバーにすると、
		// 多くのコードを変更する必要が出るため注意が必要です。
		const int MIN_STRENGTH = 1;
		const int MAX_STRENGTH = 10;

		std::shared_ptr<Board> board;
		std::shared_ptr<BoardWriter> board_writer;
		std::shared_ptr<MessageWriter> message_writer;
		ReversiBenchmark reversiBenchmark;
		ReversiEngine engine;
		InputReader reader;
		State current_state;
		Side player_turn;
		Side current_turn;
		u64 prev_input;

		std::mt19937 rand_module;

		void EnemyTurn();
		void PlayerTurn();

		void ChangeTurn();
		BoardInfo GetBoardInfo() const;
		u64 GetRandomInput();

		//ボードの更新
		void Refresh();

		void InGameLoop();

		void AskSelectTurn();
		void AskRetry();
		void AskSelectStrength();
	};
}





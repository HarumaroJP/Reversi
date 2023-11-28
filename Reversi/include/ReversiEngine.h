#pragma once

#include <thread>
#include <vector>
#include <queue>
#include "Basic.h"
#include "Board.h"
#include "Evaluator.h"
#include "SearchFuture.h"
#include "SearchResult.h"

namespace Reversi
{
	/// <summary>
	/// 最善手探索を最高効率で探索するクラス
	/// </summary>
	class ReversiEngine
	{
	public:
		explicit ReversiEngine(std::shared_ptr<Board>& board);

		//最善手を探索して取得する
		u64 MakeBestMove();

		//シングルスレッドで探索する関数
		u64 MakeBestMove_Single();

		//マルチスレッドで探索する関数
		u64 MakeBestMove_Parallel();

		int GetSearchDepth() const;
		void SetSearchDepth(const int depth);

		void SetEvaluateSide(const Reversi::Side side);
	private:

		std::shared_ptr<Board> board;
		std::vector<SearchFuture> tasks;
		std::queue<u64> input_queue;
		std::future<SearchResult> futures[64];
		SearchSystem search_system;
		Side evaluateSide;
		unsigned long long future_count;

		bool is_support_multi_thread;
		int max_depth;
	};
}





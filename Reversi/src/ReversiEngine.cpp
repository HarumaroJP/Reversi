#include "../include/ReversiEngine.h"

namespace Reversi
{
	ReversiEngine::ReversiEngine(std::shared_ptr<Board>& board) : board(board), search_system(board), max_depth(7), evaluateSide(Side::Black), future_count(0)
	{
		//サポートされるスレッド数の取得
		unsigned int support_threads_count = std::thread::hardware_concurrency();
		support_threads_count = std::min((int)support_threads_count, 64);
		is_support_multi_thread = support_threads_count > 1;

		if (is_support_multi_thread)
		{
			for (int i = 0; i < (int)support_threads_count; ++i)
			{
				tasks.emplace_back(SearchFuture());
			}
		}
	}

	void ReversiEngine::SetEvaluateSide(const Side side)
	{
		evaluateSide = side;
	}

	void ReversiEngine::SetSearchDepth(const int depth)
	{
		//探索深度の登録
		max_depth = depth;
		for (SearchFuture& task : tasks)
		{
			task.SetSearchDepth(depth);
		}
	}

	int ReversiEngine::GetSearchDepth() const
	{
		return max_depth;
	}

	u64 ReversiEngine::MakeBestMove()
	{
		return is_support_multi_thread ? MakeBestMove_Parallel() : MakeBestMove_Single();
	}

	//最善手探索のシングルスレッド版
	u64 ReversiEngine::MakeBestMove_Single()
	{
		constexpr int alpha = std::numeric_limits<int>::min();
		constexpr int beta = std::numeric_limits<int>::max();

		search_system.evaluateSide = evaluateSide;
		SearchResult info = search_system.AlphaBetaSearch(0, max_depth, alpha, beta, evaluateSide);

		return info.Point;
	}

	//最善手探索のマルチスレッド版
	u64 ReversiEngine::MakeBestMove_Parallel()
	{
		u64 legal_moves = board->GetLegalMoves(evaluateSide);
		SearchResult best_move = { std::numeric_limits<int>::min(), 0 };

		for (SearchFuture& task : tasks)
		{
			task.Initialize(*board, evaluateSide);
		}

		//着手可能場所をキューに格納
		for (int i = 0; i < 64; ++i)
		{
			u64 input = 1ull << i;

			//着手可能位置ではなかったらスキップ
			if ((legal_moves & input) == 0ull)
				continue;

			input_queue.push(input);
		}
		future_count = input_queue.size();

		//スレッド数分はとりあえず割り当てちゃう
		for (int i = 0; i < tasks.size(); ++i)
		{
			if (input_queue.empty())
				break;

			futures[i] = tasks[i].Schedule(input_queue.front());
			input_queue.pop();
		}

		auto min = std::chrono::system_clock::time_point::min();

		while (true)
		{
			for (int i = 0; i < tasks.size(); ++i)
			{
				std::future<SearchResult>& future = futures[i];
				if (!future.valid())
					continue;

				//スレッドの処理が終わっているかをチェック
				bool is_done = future.wait_until(min) == std::future_status::ready;

				if (is_done)
				{
					SearchResult result = future.get();

					//終わったらすぐに次のスケジュールを行う
					if (!input_queue.empty())
					{
						futures[i] = tasks[i].Schedule(input_queue.front());

						// popしわすれ？
						input_queue.pop();
					}

					if (result.Score > best_move.Score)
					{
						best_move = result;
					}

					//全ての処理が終わったら結果を返す
					future_count--;
					if (future_count == 0)
					{
						return best_move.Point;
					}
				}
			}
		}
	}
}





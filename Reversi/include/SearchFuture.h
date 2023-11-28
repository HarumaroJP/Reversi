#pragma once

#include <thread>
#include <functional>
#include <future>
#include "Board.h"
#include "SearchSystem.h"

namespace Reversi
{
	/// <summary>
	/// 1スレッドに対する探索を行うクラス
	/// </summary>
	class SearchFuture
	{
	public:
		explicit SearchFuture();

		void Initialize(const Board& origin, const Side side);
		void SetSearchDepth(const int depth);

		//スレッドにスケジュールする関数
		std::future<SearchResult> Schedule(const u64 input);

		//実際に探索を行う関数
		SearchResult SearchBestMove();
	private:
		int depth;
		u64 assigned_input;
		std::unique_ptr<SearchSystem> search_system;
		std::shared_ptr<Board> board_buffer;
	};
}

#include "../include/SearchFuture.h"

namespace Reversi
{
	SearchFuture::SearchFuture() : depth(7), assigned_input(0)
	{
		board_buffer = std::make_shared<Board>();
		search_system = std::make_unique<SearchSystem>(board_buffer);
	}

	void SearchFuture::Initialize(const Board& origin, const Side side)
	{
		search_system->evaluateSide = side;

		//現在のボード情報を更新する
		board_buffer->Overwrite(origin);
	}

	std::future<SearchResult> SearchFuture::Schedule(const u64 input)
	{
		assigned_input = input;

		//SearchBestMoveを非同期実行する
		return std::async(std::launch::async, &SearchFuture::SearchBestMove, this);
	}

	SearchResult SearchFuture::SearchBestMove()
	{
		Side evaluateSide = search_system->evaluateSide;
		Side nextSide = evaluateSide == Side::Black ? Side::White : Side::Black;

		constexpr int alpha = std::numeric_limits<int>::min();
		constexpr int beta = std::numeric_limits<int>::max();

		//石を配置
		board_buffer->Set(assigned_input, evaluateSide);
		u64 flips = board_buffer->Flip(assigned_input, evaluateSide);

		SearchResult info = search_system->AlphaBetaSearch(assigned_input, depth - 1, alpha, beta, nextSide);

		//手を巻き戻す
		board_buffer->SetEmpty(assigned_input);
		board_buffer->Undo(flips, evaluateSide);

		return { info.Score, assigned_input };
	}

	void SearchFuture::SetSearchDepth(const int max_depth)
	{
		this->depth = max_depth;
	}
}
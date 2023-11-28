#include "../include/SearchSystem.h"

namespace Reversi
{
	SearchSystem::SearchSystem(std::shared_ptr<Board>& board) :
		board(board),
		evaluator(board),
		evaluateSide(Side::Black)
	{

	}

	SearchResult SearchSystem::AlphaBetaSearch(const u64 point, int depth, int alpha, int beta, Side side)
	{
		// 実行速度を求めるならば、余計な処理を挟む前に評価しましょう。
		//一番深くまで到達したら評価する
		if (depth == 0)
		{
			int score = evaluator.Evaluate(evaluateSide);
			return { score, point };
		}

		u64 legal_moves = board->GetLegalMoves(side);
		bool is_max = evaluateSide == side;
		SearchResult best = { is_max ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max(), 0 };

		//おけるマスが無くなったら評価する
		if (legal_moves == 0)
		{
			int score = evaluator.Evaluate(evaluateSide);
			return { score, point };
		}

		for (int i = 0; i < 64; ++i)
		{
			u64 input = 1ull << i;

			//着手可能位置ではなかったらスキップ
			if ((legal_moves & input) == 0)
				continue;

			//探索用に設置する
			board->Set(input, side);
			u64 flips = board->Flip(input, side);

			SearchResult info = AlphaBetaSearch(input, depth - 1, alpha, beta, side == Side::Black ? Side::White : Side::Black);

			//探索が終わったら巻き戻す
			board->SetEmpty(input);
			board->Undo(flips, side);

			if (is_max)
			{
				//βカット
				if (beta <= info.Score)
					return { info.Score, input };

				if (info.Score > best.Score)
				{
					best = { info.Score, input };
					alpha = info.Score;
				}
			}
			else
			{
				//αカット
				if (alpha >= info.Score)
					return { info.Score, input };

				if (info.Score < best.Score)
				{
					best = { info.Score, input };
					beta = info.Score;
				}
			}
		}

		return best;
	}
}



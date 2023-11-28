#include "../include/GameSequencer.h"

namespace Reversi
{
	GameSequencer::GameSequencer(std::shared_ptr<Board>& board,
		std::shared_ptr<BoardWriter>& board_writer,
		std::shared_ptr<MessageWriter>& message_writer) :
		engine(board),
		player_turn(Side::Black),
		current_turn(Side::Black),
		current_state(State::Invalid),
		prev_input(0xFFFFFFFFFFFFFFFF)
	{
		this->board = board;
		this->board_writer = board_writer;
		this->message_writer = message_writer;

		rand_module.seed(1234);
	}

	void GameSequencer::Start()
	{
		// 外部に公開するものをできる限り減らしましょう
		// これら全てはこのクラス内限定使用で問題ありません。

		while (current_state != State::Stop)
		{
			//オセロボードの表示
			Refresh();

			//ターン選択
			AskSelectTurn();

			//敵の強さ選択
			AskSelectStrength();

			//ゲーム中のループ
			InGameLoop();

			if (current_state == State::End)
			{
				//リトライ選択
				AskRetry();
			}
		}
	}

	//インゲームのメインループ
	void GameSequencer::InGameLoop()
	{
		message_writer->WriteGameStartMessage();
		BoardInfo boardInfo = GetBoardInfo();

		//局面が終了するまでループ
		while (!boardInfo.is_end)
		{
			//画面の更新
			Refresh();
			message_writer->WriteTurnMessage(player_turn, engine.GetSearchDepth());

			//着手可能数を取得
			std::pair<int, int> legal_counts = board->CountLegalMoves();
			int mine_count = current_turn == Side::Black ? legal_counts.first : legal_counts.second;

			//着手出来なかったらパスする
			if (mine_count == 0)
			{
				message_writer->WritePassMessage(current_turn);
				prev_input = 0xFFFFFFFFFFFFFFFF;
			}
			else
			{
				if (current_turn == player_turn)
				{
					PlayerTurn();

					if (current_state == State::Stop || current_state == State::Restart || current_state == State::End)
						break;
				}
				else
				{
					EnemyTurn();
				}
			}

			//ボード情報を更新
			boardInfo = GetBoardInfo();

			//ターン変更
			ChangeTurn();
		}

		if (boardInfo.is_end)
		{
			current_state = State::End;

			//リザルト表示
			Refresh();
			message_writer->WriteResultMessage(boardInfo.black_count, boardInfo.white_count);
		}

		//進行状況のリセット
		board->Reset();
		current_turn = Side::Black;
		prev_input = 0xFFFFFFFFFFFFFFFF;

		//ベンチマーク用
		//reversiBenchmark.WriteResult();
		reversiBenchmark.Clear();
	}

	void GameSequencer::Refresh()
	{
		//画面のクリア
		//system("cls");

		//ボードの表示
		std::pair<u64, u64> field_data = board->GetFieldData();
		u64 legal_moves = board->GetLegalMoves(current_turn);
		board_writer->Write(field_data, legal_moves, prev_input);
	}

	void GameSequencer::ChangeTurn()
	{
		current_turn = current_turn == Side::Black ? Side::White : Side::Black;
	}

	void GameSequencer::PlayerTurn()
	{
		// doループにすることで、PlayerTurn()を呼ぶ前のcurrent_stateの初期化を減らせます。
		// whileループが好みなら、この関数内で初期化すると良いでしょう。呼び出し側の初期化忘れを減らせます。
		do
		{
			//コマンド情報を取得
			Command command = reader.ReadCommand();
			current_state = command.state;

			// ネストを深くしないように早期コンティニューさせる
			if (current_state != State::Set) continue;

			u64 legal_moves = board->GetLegalMoves(current_turn);

			//配置できる場所だったら設置
			if (command.IsLegalMove(legal_moves))
			{
				board->Set(command.set, current_turn);
				board->Flip(command.set, current_turn);
				prev_input = command.set;
			}
			else
			{
				current_state = State::Invalid;
				std::wcout << L"その場所に置くことはできません" << std::endl;
			}
		} while (current_state == State::Invalid);
	}

	void GameSequencer::EnemyTurn()
	{
		std::wcout << L"敵AI 考え中( ｰ̀ωｰ́ ).｡oஇ" << std::endl;

		//ベンチマーク
		reversiBenchmark.Start();

		//最善手を計算
		u64 best_move = engine.MakeBestMove();

		reversiBenchmark.End();

		board->Set(best_move, current_turn);
		board->Flip(best_move, current_turn);

		prev_input = best_move;
	}

	void GameSequencer::AskSelectStrength()
	{
		bool invalid = false;
		std::wstring input_buffer;

		do
		{
			message_writer->WriteSelectStrengthMessage(invalid, MIN_STRENGTH, MAX_STRENGTH);
			invalid = false;

			std::wcin >> input_buffer;

			int strength = std::stoi(input_buffer);

			if (MIN_STRENGTH <= strength && strength <= MAX_STRENGTH)
			{
				engine.SetSearchDepth(strength);
			}
			else
			{
				invalid = true;
			}
		} while (invalid);
	}

	void GameSequencer::AskSelectTurn()
	{
		bool invalid = false;
		std::wstring input_buffer;

		do
		{
			message_writer->WriteSelectTurnMessage(invalid);
			invalid = false;

			std::wcin >> input_buffer;

			if (input_buffer == L"0")
			{
				player_turn = Side::Black;
				engine.SetEvaluateSide(Side::White);
			}
			else if (input_buffer == L"1")
			{
				player_turn = Side::White;
				engine.SetEvaluateSide(Side::Black);
			}
			else
			{
				invalid = true;
			}
		} while (invalid);
	}

	BoardInfo GameSequencer::GetBoardInfo() const
	{
		std::pair<int, int> stoneCounts = board->CountStone();

		//盤面が全て埋まっているか or 一手でも互いにおけるマスがあるか
		bool is_end = board->GetAllBoard() == 0xFFFFFFFFFFFFFFFF
			|| (board->GetLegalMoves(Side::Black) | board->GetLegalMoves(Side::White)) == 0ull;

		return { stoneCounts.first, stoneCounts.second, is_end };
	}

	void GameSequencer::AskRetry()
	{
		bool invalid = false;
		std::wstring input_buffer;

		do
		{
			message_writer->WriteRetryMessage(invalid);
			invalid = false;

			std::wcin >> input_buffer;

			if (input_buffer == L"y")
			{
				current_state = State::Restart;
			}
			else if (input_buffer == L"n")
			{
				current_state = State::Stop;
			}
			else
			{
				input_buffer.clear();
				invalid = true;
			}
		} while (invalid);
	}

	u64 GameSequencer::GetRandomInput()
	{
		std::vector<unsigned char> legal_indexes;
		u64 legal_moves = board->GetLegalMoves(current_turn);

		for (int i = 0; i < 64; ++i)
		{
			u64 input = 1ull << i;

			//着手可能位置ではなかったらスキップ
			if ((legal_moves & input) == 0)
				continue;

			legal_indexes.emplace_back(i);
		}

		std::shuffle(legal_indexes.begin(), legal_indexes.end(), rand_module);

		return 1ull << legal_indexes[0];
	}
}
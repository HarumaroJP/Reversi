#include "../include/InputReader.h"

namespace Reversi
{
	bool InputReader::IsLegalCommand(const std::wstring& command) const
	{
		// コード量が長くなるため、好みの問題ですが、
		// 下記のようにすると判定している部分が直感的にわかり、可読性が上がります。
		// また、内部処理では現状と同じ小文字で判定する形式にして、
		// プレイヤーの入力は大文字を許可してもいいと思います。
		bool isCulumnAtoH = (L'a' <= command[0] && command[0] <= L'h') || (L'A' <= command[0] && command[0] <= L'H');
		bool isRow1To8 = L'1' <= command[1] && command[1] <= L'8';

		return isCulumnAtoH && isRow1To8;
	}

	bool Command::IsLegalMove(Reversi::u64 legal_positions) const
	{
		return (set & legal_positions) == set;
	}

	Command InputReader::ReadCommand() const
	{
		std::wstring input;
		State state;
		u64 pos = 0ull;

		WriteInitialMessage();

		//入力を取る
		std::wcin >> input;

		size_t length = input.length();

		if (length == 2)
		{
			if (IsLegalCommand(input))
			{
				state = State::Set;

				// 大文字を小文字に変換
				input[0] = std::tolower(input[0]);

				pos = CommandToBoard(input);
				std::wcout << std::endl;
			}
			else
			{
				state = State::Invalid;
				std::wcout << L"1文字目はa~h, 2文字目は1~8で入力してください。\n\n";
			}
		}
		else if (length == 1 && input[0] == L'r')
		{
			state = State::Restart;
		}
		else if (length == 1 && input[0] == L's')
		{
			state = State::Stop;
		}
		else
		{
			state = State::Invalid;
			std::wcout << L"存在しないコマンドです。\n\n";
		}

		return { state, pos };
	}

	void InputReader::WriteInitialMessage() const
	{
		std::wcout << L"コマンドを入力してください" << std::endl;
		std::wcout << L"石を配置: 横列+縦列(例: a2)" << std::endl;
		std::wcout << L"リスタート: r" << std::endl;
		std::wcout << L"終了: s" << std::endl;
		std::wcout << L">> ";
	}

	u64 InputReader::CommandToBoard(const std::wstring& command) const
	{
		int col = command[0] - L'a';
		int row = command[1] - L'1';

		return 1ull << (8 * row + col);
	}
}

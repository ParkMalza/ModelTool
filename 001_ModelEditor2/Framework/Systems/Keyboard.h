#pragma once

#define MAX_INPUT_KEY 255
#define MAX_INPUT_MOUSE 8

class Keyboard
{
public:
	static Keyboard* Get();

	static void Create();
	static void Delete();

	void Update();

	bool Down(DWORD key) { return keyMap[key] == KEY_INPUT_STATUS_DOWN; }  //������ �´ٸ� 1��ȯ, �ٸ��ٸ� 0 ��ȯ...
	bool Up(DWORD key) { return keyMap[key] == KEY_INPUT_STATUS_UP; }
	bool Press(DWORD key) { return keyMap[key] == KEY_INPUT_STATUS_PRESS; }

private:
	Keyboard();
	~Keyboard();

	static Keyboard* instance;

	byte keyState[MAX_INPUT_KEY];
	byte keyOldState[MAX_INPUT_KEY];
	byte keyMap[MAX_INPUT_KEY];

	enum
	{
		KEY_INPUT_STATUS_NONE = 0,  //..
		KEY_INPUT_STATUS_DOWN,  //Ű���� ����
		KEY_INPUT_STATUS_UP,  //Ű���� ��
		KEY_INPUT_STATUS_PRESS,  //Ű���� ��������
	};
};
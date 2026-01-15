#include "Input.h"
#include <cassert>

//インスタンスのゲッター
Input* Input::GetInstance() {
	assert(!isFinalize && "GetInstance() called after Finalize()");
	if (instance == nullptr) {
		instance = new Input();
	}
	return instance;
}

//更新
void Input::Update() {
	memcpy(preKeys_, keys_, 256);
	Novice::GetHitKeyStateAll(keys_);
}

//終了
void Input::Finalize() {
	delete instance;
	instance = nullptr;
	isFinalize = true;
}

//Pressキーのゲッター
bool Input::GetPressKeys(int32_t keyNumber){
	//指定キーを押していればtrueを返す
	if (keys_[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

//Triggerきーのゲッター
bool Input::GetTriggerKeys(int32_t keyNumber){
	//指定キーを押していればtrueを返す
	if (keys_[keyNumber]&&preKeys_[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

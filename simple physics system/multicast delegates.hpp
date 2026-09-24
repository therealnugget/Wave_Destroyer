#pragma once
#include <functional>
#include <tuple>
#include "usefulTypedefs.hpp"
#include "main.hpp"
template<typename T>
class MultiDelegate {
private:
	Node<std::function<void(T)>>* curNode;
	Node<std::function<void(T)>>* delegates = nullptr;
	bool removedOnFrame = false;
public:
	inline Node<std::function<void(T)>>* operator +=(std::function<void(T)> del) {
		return Node<std::function<void(T)>>::AddAtHead(del, &delegates);
	}
	inline Node<std::function<void(T)>>* AddAtTail(std::function<void(T)> del) {
		return Node<std::function<void(T)>>::AddAtTail(del, &delegates);
	}
	inline void operator -=(Node<std::function<void(T)>>* node) {
		if (curNode == node) {
			Node<std::function<void(T)>>::Advance(&curNode);
			removedOnFrame = true;
		}
		Node<std::function<void(T)>>::Remove(&delegates, node);
	}
	void operator ()(T);
};
template<>
class MultiDelegate<void>;
template<>
class MultiDelegate<void> {
private:
	Node<std::function<void(void)>>* curNode;
	Node<std::function<void(void)>> *delegates = nullptr;
	bool removedOnFrame = false;
public:
	inline Node<std::function<void(void)>>* operator +=(std::function<void(void)> del) {
		return Node<std::function<void(void)>>::AddAtHead(del, &delegates);
	}
	inline Node<std::function<void(void)>>* AddAtTail(std::function<void(void)> del) {
		return Node<std::function<void(void)>>::AddAtTail(del, &delegates);
	}
	inline void operator -=(Node<std::function<void(void)>>* node) {
		if (curNode == node) {
			Node<std::function<void(void)>>::Advance(&curNode);
			removedOnFrame = true;
		}
		Node<std::function<void(void)>>::Remove(&delegates, node);
	}
	void operator ()(void);
};
template <typename T>
void MultiDelegate<T>::operator ()(T t) {
	curNode = delegates;
	while (curNode) {
		curNode->value(t);
		if (removedOnFrame) {
			removedOnFrame = false;
			continue;
		}
		Node<std::function<void(T)>>::Advance(&curNode);
	}
}
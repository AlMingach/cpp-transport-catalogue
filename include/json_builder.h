#include "json.h"

namespace json {

	class ItemContext;
	class KeyItemContext;
	class DictItemContext;
	class ArrayItemContext;

	class Builder {

	public:

		Builder() = default;

		KeyItemContext Key(std::string);

		Builder& Value(Node, bool = false);

		DictItemContext StartDict();

		ArrayItemContext StartArray();

		Builder& EndDict();

		Builder& EndArray();

		Node Build();

	private:
		Node node_;
		std::vector<Node*> nodes_stack_;

		bool is_empty_ = true;
	};

	class ItemContext {

	public:
		ItemContext(Builder& builder)
			:builder_(builder)
		{
		}

	protected:

		KeyItemContext Key(std::string);

		DictItemContext StartDict();

		ArrayItemContext StartArray();

		Builder& EndDict();

		Builder& EndArray();

		Builder& builder_;
	};

	class ArrayItemContext : public ItemContext {
	public:
		using ItemContext::ItemContext;
		using ItemContext::StartDict;
		using ItemContext::StartArray;
		using ItemContext::EndArray;

		ArrayItemContext Value(Node value);

	};

	class DictItemContext : public ItemContext {
	public:
		using ItemContext::ItemContext;
		using ItemContext::Key;
		using ItemContext::EndDict;
	};

	class KeyItemContext : public ItemContext {
	public:
		using ItemContext::ItemContext;
		using ItemContext::StartDict;
		using ItemContext::StartArray;

		DictItemContext Value(Node value);

	};

}
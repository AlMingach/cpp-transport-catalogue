#include "json_builder.h"

namespace json {

	using namespace std::literals;

	KeyItemContext Builder::Key(std::string key) {
		if (!is_empty_ && !nodes_stack_.empty() && nodes_stack_.back()->IsDict()) {
			nodes_stack_.emplace_back(&const_cast<Dict&>(nodes_stack_.back()->AsDict())[std::move(key)]);
			return *this;
		}

		throw std::logic_error("Incorrect place for key : "s + key);
	}

	Builder& Builder::Value(Node value, bool start) {

		if (is_empty_) {
			is_empty_ = false;
			node_ = std::move(value); 
			if (start) {
				nodes_stack_.push_back(&node_);
			}
			return *this;
		}

		if (!nodes_stack_.empty()) {
			if (nodes_stack_.back()->IsNull()) {
				*nodes_stack_.back() = std::move(value); 
				if (!start) {
					nodes_stack_.pop_back();
				}
				return *this;
			}

			if (nodes_stack_.back()->IsArray()) {
				auto& p = const_cast<Array&>(nodes_stack_.back()->AsArray());
				p.push_back(value); 
				if (start) {
					nodes_stack_.push_back(&p.back());
				}
				return *this;
			}
		}

		throw std::logic_error("Incorrect place for value"s);
	}

	DictItemContext Builder::StartDict() {
		Value(Dict{}, true);
		return *this;
	}

	ArrayItemContext Builder::StartArray() {
		Value(Array{}, true);
		return *this;
	}

	Builder& Builder::EndDict() {
		if (!is_empty_ && !nodes_stack_.empty() && nodes_stack_.back()->IsDict()) {
			nodes_stack_.pop_back();
			return *this;
		}
		throw std::logic_error("Incorrect place for EndDict"s);
	}

	Builder& Builder::EndArray() {
		if (!is_empty_ && !nodes_stack_.empty() && nodes_stack_.back()->IsArray()) {
			nodes_stack_.pop_back();
			return *this;
		}
		throw std::logic_error("Incorrect place for EndArray"s);
	}

	Node Builder::Build() {
		if (!is_empty_ &&  nodes_stack_.empty()) {
			return std::move(node_);
		}
		throw std::logic_error("Builder is invalid"s);
	}
	//..............................ItemContext........................................

	DictItemContext ItemContext::StartDict() {
		return builder_.StartDict();
	}

	ArrayItemContext ItemContext::StartArray() {
		return builder_.StartArray();
	}

	Builder& ItemContext::EndArray() {
		return builder_.EndArray();
	}

	ArrayItemContext ArrayItemContext::Value(Node value) {
		return builder_.Value(value);
	}

	KeyItemContext ItemContext::Key(std::string key) {
		return builder_.Key(std::move(key));
	}

	Builder& ItemContext::EndDict() {
		return builder_.EndDict();
	}

	DictItemContext KeyItemContext::Value(Node value) {
		return builder_.Value(value);
	}
}
#include "json_builder.h"
namespace transport_catalogue {
namespace detail {
namespace json {

KeyItemContext Builder::Key(std::string key){
    if (nodes_stack_.empty()) {
        throw std::logic_error("unable to create key");
    }
    auto keyp = std::make_unique<Node>(key);
    if (nodes_stack_.back()->IsDict()) {
        nodes_stack_.emplace_back(std::move(keyp));
    }

    return KeyItemContext(*this);
}

Node Builder::CreateNode(Node::Value val){
    Node node;

    if(std::holds_alternative<Dict>(val)){
        node = Node(std::get<Dict>(val));
    } else if(std::holds_alternative<Array>(val)){
        node = Node(std::get<Array>(val));
    } else if(std::holds_alternative<int>(val)){
        node = Node(std::get<int>(val));
    } else if(std::holds_alternative<double>(val)){
        node = Node(std::get<double>(val));
    } else if(std::holds_alternative<std::string>(val)){
        node = Node(std::get<std::string>(val));
    } else if(std::holds_alternative<bool>(val)){
        node = Node (std::get<bool>(val));
    } else {
        node = Node();
    }
    return node;

}

void Builder::AddNode(Node node){
    if (!root_.IsNull()) {
        throw std::logic_error("root has been added");
    }

    if(nodes_stack_.empty()){
        root_ = node;
    } else if(nodes_stack_.back()->IsString()){
        std::string key = nodes_stack_.back()->AsString();
        nodes_stack_.pop_back();
                if(nodes_stack_.back()->IsDict()){
                    Dict dict = nodes_stack_.back()->AsDict();
                    dict.emplace(key, node);
                    auto dict_ptr = std::make_unique<Node>(dict);
                    nodes_stack_.pop_back();
                    nodes_stack_.emplace_back(std::move(dict_ptr));
                } else {
                    throw std::logic_error("bad to create node");
                }
        } else if(nodes_stack_.back()->IsArray()){
            Array array = nodes_stack_.back()->AsArray();
            array.emplace_back(node);
            nodes_stack_.pop_back();
            auto arr_ptr = std::make_unique<Node>(array);
            nodes_stack_.emplace_back(std::move(arr_ptr));
        } else {
            throw std::logic_error("bad to create node");
        }
    }



Builder& Builder::Value(Node::Value val){

    Builder::AddNode(CreateNode(val));
    return *this;
}

DictItemContext Builder::StartDict(){
    nodes_stack_.emplace_back(std::make_unique<Node>(Dict()));
    return DictItemContext(*this);
}

Builder& Builder::EndDict(){
    if (nodes_stack_.empty()) {
        throw std::logic_error("unable to close as without opening");
    }
    Node node = *nodes_stack_.back();

    if (!node.IsDict()) {
        throw std::logic_error("object isn't dictionary");
    }

    nodes_stack_.pop_back();
    AddNode(node);

    return *this;
}

ArrayItemContext Builder::StartArray(){
    nodes_stack_.emplace_back(std::make_unique<Node>(Array()));
    return ArrayItemContext(*this);
}
Builder& Builder::EndArray(){
    if (nodes_stack_.empty()) {
        throw std::logic_error("unable to close without opening");
    }

    Node node = *nodes_stack_.back();

    if (!node.IsArray()) {
        throw std::logic_error("object isn't array");
    }

    nodes_stack_.pop_back();
    AddNode(node);

    return *this;
}

Node Builder::Build() {

    if (root_.IsNull()) {
        throw std::logic_error("empty json");
    }

    if (!nodes_stack_.empty()) {
        throw std::logic_error("invalid json");
    }

    return root_;
}


//----------------------------DictItemContext----------------------

DictItemContext::DictItemContext(Builder& bild) : builder_(bild){}


KeyItemContext DictItemContext::Key(std::string key){
    return builder_.Key(key);
}
Builder& DictItemContext::EndDict(){
    return builder_.EndDict();
}

//----------------------------KeyItemContext---------------------
KeyItemContext::KeyItemContext(Builder& bild): builder_(bild){}

ArrayItemContext KeyItemContext::StartArray(){
    return builder_.StartArray();
}
DictItemContext KeyItemContext::Value(Node::Value val){
    return builder_.Value(val);
}
DictItemContext KeyItemContext::StartDict(){
    return builder_.StartDict();
}

//----------------------------ArrayItemContext----------------------------------

ArrayItemContext::ArrayItemContext(Builder& bild): builder_(bild){}

ArrayItemContext ArrayItemContext::Value(Node::Value val){
    return builder_.Value(val);
}
DictItemContext ArrayItemContext::StartDict(){
    return builder_.StartDict();
}
ArrayItemContext ArrayItemContext::StartArray(){
    return builder_.StartArray();
}
Builder& ArrayItemContext::EndArray(){
    return builder_.EndArray();
}

}//end namespace json
}
}

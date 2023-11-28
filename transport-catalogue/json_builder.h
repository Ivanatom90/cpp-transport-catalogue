#pragma once
#include "json.h"
#include <memory>

namespace transport_catalogue {
namespace detail {
namespace json {

class DictItemContext;
class KeyItemContext;
class ArrayItemContext;


class Builder{

public:
    KeyItemContext Key(std::string key); //При определении словаря задаёт строковое значение ключа для очередной пары ключ-значение. Следующий вызов метода обязательно должен задавать соответствующее этому ключу значение с помощью метода Value или начинать его определение с помощью StartDict или StartArray.
    Builder& Value(Node::Value val); //Задаёт значение, соответствующее ключу при определении словаря
    DictItemContext StartDict(); //Начинает определение сложного значения-словаря. Вызывается в тех же контекстах, что и Value. Следующим вызовом обязательно должен быть Key или EndDict
    Builder& EndDict();
    ArrayItemContext StartArray(); //Начинает определение сложного значения-массива
    Builder& EndArray();
    Node Build();

private:
    Node CreateNode(Node::Value val);
    void AddNode(Node node);
    Node root_;
    std::vector<std::unique_ptr<Node>> nodes_stack_;
};


class DictItemContext : public Builder{
  public:
    DictItemContext(Builder& bild);

    KeyItemContext Key(std::string key);
    Builder& EndDict();
    Builder& Value(Node::Value val) = delete; //Задаёт значение, соответствующее ключу при определении словаря
    Node CreateNode(Node::Value val) = delete;
    DictItemContext StartDict() = delete; //Начинает определение сложного значения-словаря. Вызывается в тех же контекстах, что и Value. Следующим вызовом обязательно должен быть Key или EndDict
    ArrayItemContext StartArray() = delete; //Начинает определение сложного значения-массива
    Builder& EndArray() = delete;
    void AddNode(Node node) = delete;

  private:
    using   Builder::EndDict, Builder::Build;
    Builder& builder_;
};



class KeyItemContext : public Builder{
  public:
    KeyItemContext(Builder& bild);

    ArrayItemContext StartArray();
    DictItemContext Value(Node::Value val);
    DictItemContext StartDict();

  private:
    using Builder::EndArray, Builder::EndDict, Builder::Key, Builder::Build;
    Builder& builder_;
};

class ArrayItemContext : public Builder{
  public:
    ArrayItemContext(Builder& bild);

    ArrayItemContext Value(Node::Value val);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndArray();

  private:
    using Builder::EndDict, Builder::Key, Builder::Build;
    Builder& builder_;
};


} // end json
}
}

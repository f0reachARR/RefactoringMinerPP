#pragma once

#include <set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Serializable
{
  public:
    virtual json toJson() = 0;
    virtual ~Serializable() = default;

  protected:
    template<typename T>
    static std::vector<json> toJsonArray(std::vector<T*>& v)
    {
      std::vector<json> result(v.size());

      std::transform(v.begin(), v.end(), result.begin(), [](T* s)
      {
        return toJsonObj(dynamic_cast<Serializable*>(s));
      });

      return result;
    }

    template<typename T>
    static std::vector<json> toJsonArrayRev(std::vector<T*>& v)
    {
      std::vector<json> result(v.size());

      std::transform(v.rbegin(), v.rend(), result.begin(), [](T* s)
      {
        return toJsonObj(dynamic_cast<Serializable*>(s));
      });

      return result;
    }

    template<typename T>
    static std::vector<json> toJsonArray(std::set<T*>& v)
    {
      std::vector<T> result(v.begin(), v.end());

      return toJsonArray<T>(result);
    }

    static json toJsonObj(Serializable* s)
    {
      if (s == nullptr)
        return nullptr;

      return s->toJson();
    }
};
// #include "keyword.h"
// #include "common/system/nameof.hpp"

// QString KeyWord::ToString(Word v)
// {
//     // Neargye::nameof_enum → string_view
//     auto sv = nameof::nameof_enum(v);
//     return QString::fromUtf8(sv.data(), sv.size());
// }

// KeyWord::Word KeyWord::Parse(const QString& p)
// {
//     QString s = p.trimmed().toLower();

//     if (s == "avg") return avg;
//     if (s == "sum") return sum;
//     if (s == "sub") return sub;
//     if (s == "add") return add;
//     if (s == "mul") return mul;
//     if (s == "div") return div;
//     if (s == "qty_fixed")    return qty_fixed;
//     if (s == "qty_perOrder") return qty_perOrder;
//     if (s == "qty_perArea")  return qty_perArea;

//     return Undefined;
// }


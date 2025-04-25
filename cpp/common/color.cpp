#include "color.hpp"

namespace cgfs
{

//                                   R     G     B     A
const Color Color::kBlack        = { 0.0f, 0.0f, 0.0f, 1.0f };
const Color Color::kWhite        = { 1.0f, 1.0f, 1.0f, 1.0f };
const Color Color::kGray         = { 0.5f, 0.5f, 0.5f, 1.0f };
const Color Color::kBrightRed    = { 1.0f, 0.0f, 0.0f, 1.0f };
const Color Color::kBrightGreen  = { 0.0f, 1.0f, 0.0f, 1.0f };
const Color Color::kBrightBlue   = { 0.0f, 0.0f, 1.0f, 1.0f };
const Color Color::kBrightYellow = { 1.0f, 1.0f, 0.0f, 1.0f };
const Color Color::kDarkRed      = { 0.5f, 0.0f, 0.0f, 1.0f };
const Color Color::kDarkGreen    = { 0.0f, 0.5f, 0.0f, 1.0f };
const Color Color::kDarkBlue     = { 0.0f, 0.0f, 0.5f, 1.0f };
const Color Color::kDarkYellow   = { 0.5f, 0.5f, 0.0f, 1.0f };
const Color Color::kCyan         = { 0.0f, 1.0f, 1.0f, 1.0f };
const Color Color::kPurple       = { 1.0f, 0.0f, 1.0f, 1.0f };

} // cgfs

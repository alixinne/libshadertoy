#include <utility>

#include "shadertoy/compiler/file_part.hpp"
#include "shadertoy/compiler/template_error.hpp"

#include "shadertoy/utils/assert.hpp"

#include "read_contents.hpp"

using namespace shadertoy::compiler;
using namespace shadertoy::utils;

file_part::file_part(const std::string &name) : cloneable_part(name) {}

file_part::file_part(const std::string &name, std::string source_file)
: cloneable_part(name), source_file_(std::move(source_file))
{
}

file_part::operator bool() const { return !source_file_.empty(); }

std::vector<std::pair<std::string, std::string>> file_part::sources() const
{
	std::vector<std::pair<std::string, std::string>> result;
	throw_assert<template_error>(!source_file_.empty(), "Template part {} is not specified", name());

	log::shadertoy()->trace("Reading {}", source_file_);
	result.emplace_back(source_file_, read_contents(source_file_));
	return result;
}

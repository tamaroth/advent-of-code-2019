#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../day.hpp"


class Instruction {
public:
	class Parameters {
	public:
		int first;
		int second;
		int output;
	};
	template<typename IAddress>
	static Instruction& read_instruction(IAddress& addr) {
		static Instruction op;
		op.opcode = *addr++;
		op.parameters.first = *addr++;
		op.parameters.second = *addr++;
		op.parameters.output = *addr++;
		return op;
	}

public:
	int opcode;
	Parameters parameters;

};

class Alarm : public Day {
public:
	virtual std::string part_01() override;
	virtual std::string part_02() override;

	static std::unique_ptr<Day> create();
	static std::string name();

private:
	std::vector<int> get_program_from_string(const std::string& source);
	void patch_program(std::vector<int>& source, std::vector<int> patch);
	void run_program(std::vector<int>& source);

private:
	static bool s_registered;
	#include "puzzle_input"
};

#pragma once

#include <cstdint>

#include <condition_variable>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace intcode {

// Supported instruction types.
enum class Type {
	ADD = 1,
	MULTIPLY = 2,
	INPUT = 3,
	OUTPUT = 4,
	JNZ = 5,
	JZ = 6,
	LT = 7,
	EQ = 8,
	BASE = 9,
	STOP = 99,
};

// Forward declarations.
class Computer;
class CPU;
class Instruction;
class Parameter;

// Types definitions.
using Value = int64_t;
using Data = std::deque<Value>;
using Memory = std::vector<Value>;
using Parameters = std::vector<Parameter>;
using Pointer = Memory::iterator;
using Program = std::map<Memory::size_type, Computer>;
using Hooks = std::map<Type, std::function<void(Program&, Computer&, std::unique_ptr<Instruction>&)>>;

// Instruction parameters.
class Parameter {
public:
	enum class Mode: Value {
		POSITION,
		IMMEDIATE,
		RELATIVE,
	};

public:
	Parameter(Value value, Value mode);
	~Parameter() = default;

	Value value;
	Mode mode;
};

// Central processing unit.
class CPU {
public:
	CPU(Memory& memory, Data& input, Data& output, Value base);
	~CPU() = default;
	void memory_size_guard(Value location);

	Memory& memory;
	Data& input;
	Data& output;
	Value base;
};

// Generic instruction.
class Instruction {
public:
	Instruction(CPU& cpu, Parameters parameters, Value ip);
	virtual ~Instruction() = default;
	virtual void execute(CPU& cpu) = 0;
	virtual Type get_opcode() const = 0;
	virtual Value get_ip() const;

protected:
	template<typename TReturnType>
	static std::unique_ptr<TReturnType> from_source(
			CPU& cpu, Value modes, Value ip, int parameter_count) {
		std::vector<Parameter> parameters;
		while (parameter_count--) {
			parameters.push_back(Parameter(cpu.memory.at(ip), modes % 10));
			ip++;
			modes /= 10;
		}
		return std::make_unique<TReturnType>(cpu, parameters, ip);
	}

	Value get_parameter_with_mode(CPU& cpu, int parameter_id);
	void store_to_parameter(CPU& cpu, int parameter_id, Value value);

	// Instructions with 3 parameters.
	template<typename Operation>
	void execute_instruction(CPU& cpu, Operation&& op) {
		store_to_parameter(
			cpu,
			2,
			op(
				get_parameter_with_mode(cpu, 0),
				get_parameter_with_mode(cpu, 1)
			)
		);
	}

	Value ip;
	Parameters parameters;
	CPU& cpu;
};

std::unique_ptr<Instruction> instruction_factory(CPU& cpu, Value ip);

class Add: public Instruction {
public:
	Add(CPU& cpu, Parameters parameters, Value ip);
	virtual ~Add() = default;
	virtual void execute(CPU& cpu) override;
	virtual Type get_opcode() const override { return Type::ADD; }
	static std::unique_ptr<Add> from_source(CPU& cpu, Value modes, Value ip);
};

class Multiply : public Instruction {
public:
	Multiply(CPU& cpu, Parameters parameters, Value ip);
	virtual ~Multiply() = default;
	virtual void execute(CPU& cpu) override;
	virtual Type get_opcode() const override { return Type::MULTIPLY; }
	static std::unique_ptr<Multiply> from_source(CPU& cpu, Value modes, Value ip);
};

class Input : public Instruction {
public:
	Input(CPU& cpu, Parameters parameters, Value ip);
	virtual ~Input() = default;
	virtual void execute(CPU& cpu) override;
	virtual Type get_opcode() const override { return Type::INPUT; }
	static std::unique_ptr<Input> from_source(CPU& cpu, Value modes, Value ip);
};

class Output : public Instruction {
public:
	Output(CPU& cpu, Parameters parameters, Value ip);
	virtual ~Output() = default;
	virtual void execute(CPU& cpu) override;
	virtual Type get_opcode() const override { return Type::OUTPUT; }
	static std::unique_ptr<Output> from_source(CPU& cpu, Value modes, Value ip);
};

class JumpTrue : public Instruction {
public:
	JumpTrue(CPU& cpu, Parameters parameters, Value ip);
	virtual ~JumpTrue() = default;
	virtual void execute(CPU& cpu) override;
	virtual Type get_opcode() const override { return Type::JNZ; }
	static std::unique_ptr<JumpTrue> from_source(CPU& cpu, Value modes, Value ip);
};

class JumpFalse : public Instruction {
public:
	JumpFalse(CPU& cpu, Parameters parameters, Value ip);
	virtual ~JumpFalse() = default;
	virtual void execute(CPU& cpu) override;
	virtual Type get_opcode() const override { return Type::JZ; }
	static std::unique_ptr<JumpFalse> from_source(CPU& cpu, Value modes, Value ip);
};

class LessThan : public Instruction {
public:
	LessThan(CPU& cpu, Parameters parameters, Value ip);
	virtual ~LessThan() = default;
	virtual void execute(CPU& cpu) override;
	virtual Type get_opcode() const override { return Type::LT; }
	static std::unique_ptr<LessThan> from_source(CPU& cpu, Value modes, Value ip);
};

class Equal : public Instruction {
public:
	Equal(CPU& cpu, Parameters parameters, Value ip);
	virtual ~Equal() = default;
	virtual void execute(CPU& cpu) override;
	virtual Type get_opcode() const override { return Type::EQ; }
	static std::unique_ptr<Equal> from_source(CPU& cpu, Value modes, Value ip);
};

class Base : public Instruction {
public:
	Base(CPU& cpu, Parameters parameters, Value ip);
	virtual ~Base() = default;
	virtual void execute(CPU& cpu) override;
	virtual Type get_opcode() const override { return Type::BASE; }
	static std::unique_ptr<Base> from_source(CPU& cpu, Value modes, Value ip);
};

class Stop : public Instruction {
public:
	Stop(CPU& cpu, Parameters parameters, Value ip);
	virtual ~Stop() = default;
	virtual void execute(CPU& cpu) override;
	virtual Type get_opcode() const override { return Type::STOP; }
	static std::unique_ptr<Stop> from_source(CPU& cpu, Value modes, Value ip);
};

class Computer {
public:
	Computer(Memory memory, Data initial_input, Memory::size_type feed_to);

public:
	void add_to_input(Value value);
	void wait_for_input();
	Value wait_for_input_and_pop();

private:
	Memory memory;
	Data input;
	Data output;
	std::mutex m;
	std::condition_variable cv;
public:
	Memory::size_type feed_to;
	CPU cpu;
};

// Code execution.
void run_program_on_computer_with_id(Program& program, Memory::size_type id, Hooks instruction_hooks = {});
void run_program_with_phase_settings(Program& program, Memory phase_settings, Hooks instruction_hooks = {});
Program get_program_for_memory_with_phase_settings(const Memory& memory, const Memory& phase_settings);
Program get_program_for_memory_with_patched_data(const Memory& memory, const Memory& patch);
Program get_program_for_memory_with_input_data(const Memory& memory, const Data& data);
Memory get_memory_from_string(const std::string& source);

} // intcode

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

	Memory& memory;
	Data& input;
	Data& output;
	Value base;
};

// Generic instruction.
class Instruction {
public:
	Instruction(Pointer& pointer, Parameters parameters);
	virtual ~Instruction() = default;
	virtual void execute(CPU& cpu) = 0;

protected:
	template<typename TReturnType>
	static std::unique_ptr<TReturnType> from_source(
			Pointer& source, Value modes, int parameter_count) {
		std::vector<Parameter> parameters;
		while (parameter_count--) {
			parameters.push_back(Parameter(*source++, modes % 10));
			modes /= 10;
		}
		return std::make_unique<TReturnType>(source, parameters);
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

	Pointer& m_pointer;
	Parameters m_parameters;
};

class Add: public Instruction {
public:
	Add(Pointer& pointer, Parameters parameters);
	virtual ~Add() = default;
	virtual void execute(CPU& cpu) override;
	static std::unique_ptr<Add> from_source(Pointer& source, Value modes);
};

class Multiply : public Instruction {
public:
	Multiply(Pointer& pointer, Parameters parameters);
	virtual ~Multiply() = default;
	virtual void execute(CPU& cpu) override;
	static std::unique_ptr<Multiply> from_source(Pointer& source, Value modes);
};

class Input : public Instruction {
public:
	Input(Pointer& pointer, Parameters parameters);
	virtual ~Input() = default;
	virtual void execute(CPU& cpu) override;
	static std::unique_ptr<Input> from_source(Pointer& source, Value modes);
};

class Output : public Instruction {
public:
	Output(Pointer& pointer, Parameters parameters);
	virtual ~Output() = default;
	virtual void execute(CPU& cpu) override;
	static std::unique_ptr<Output> from_source(Pointer& source, Value modes);
};

class JumpTrue : public Instruction {
public:
	JumpTrue(Pointer& pointer, Parameters parameters);
	virtual ~JumpTrue() = default;
	virtual void execute(CPU& cpu) override;
	static std::unique_ptr<JumpTrue> from_source(Pointer& source, Value modes);
};

class JumpFalse : public Instruction {
public:
	JumpFalse(Pointer& pointer, Parameters parameters);
	virtual ~JumpFalse() = default;
	virtual void execute(CPU& cpu) override;
	static std::unique_ptr<JumpFalse> from_source(Pointer& source, Value modes);
};

class LessThan : public Instruction {
public:
	LessThan(Pointer& pointer, Parameters parameters);
	virtual ~LessThan() = default;
	virtual void execute(CPU& cpu) override;
	static std::unique_ptr<LessThan> from_source(Pointer& source, Value modes);
};

class Equal : public Instruction {
public:
	Equal(Pointer& pointer, Parameters parameters);
	virtual ~Equal() = default;
	virtual void execute(CPU& cpu) override;
	static std::unique_ptr<Equal> from_source(Pointer& source, Value modes);
};

class Stop : public Instruction {
public:
	Stop(Pointer& pointer, Parameters parameters);
	virtual ~Stop() = default;
	virtual void execute(CPU& cpu) override;
	static std::unique_ptr<Stop> from_source(Pointer& source, Value modes);
};

class Computer {
public:
	Computer(Memory memory, Data initial_input, Memory::size_type feed_to);

public:
	void add_to_input(Value value);
	void wait_for_input();

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

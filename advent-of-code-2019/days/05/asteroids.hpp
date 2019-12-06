#pragma once

#include <map>

#include "../day.hpp"

class Parameter;

using Parameters = std::vector<Parameter>;
using Memory = std::vector<int>;
using Pointer = Memory::iterator;

enum class Type {
	ADD = 1,
	MULTIPLY = 2,
	INPUT = 3,
	OUTPUT = 4,
	JNZ = 5,
	JZ = 6,
	LT = 7,
	EQ = 8,
	STOP = 99
};

class Parameter {
public:
    enum class Mode: int {
        POSITION,
        IMMEDIATE,
    };
    Parameter(int value, int mode);
    ~Parameter() = default;

    int value;
    Mode mode;
};

class Instruction {
public:
	

public:
    Instruction(Pointer& pointer, Parameters parameters);
    virtual ~Instruction() = default;
    virtual void execute(std::vector<int>& source) = 0;

protected:
    template<typename TReturnType, typename ISource>
    static std::unique_ptr<TReturnType> from_source(ISource& source, int modes, int parameter_count) {
        std::vector<Parameter> parameters;
        while (parameter_count--) {
            parameters.push_back(
                // This here is fucky, because what happens when source reaches end?
                Parameter(*source++, modes % 10)
            );
            modes /= 10;
        }
        return std::make_unique<TReturnType>(source, parameters);
    }

    int get_parameter_with_mode(Memory& memory, int parameter_id);
    void store_to_parameter(Memory& memory, int parameter_id, int value);

    // Instructions with 3 parameters.
    template<typename Operation>
    void execute_instruction(Memory& memory, Operation&& op) {
        store_to_parameter(
            memory,
            2,
            op(
                get_parameter_with_mode(memory, 0),
                get_parameter_with_mode(memory, 1)
            )
        );
    }

    std::vector<Parameter> m_parameters;
    Pointer& m_pointer;
};

class Add: public Instruction {
public:
    Add(Pointer& pointer, Parameters parameters);
    virtual ~Add() = default;
    virtual void execute(Memory& memory) override;

    template<typename ISource>
    static std::unique_ptr<Add> from_source(ISource& source, int modes) {
        return Instruction::from_source<Add>(source, modes, 3);
    }
};

class Multiply : public Instruction {
public:
    Multiply(Pointer& pointer, Parameters parameters);
    virtual ~Multiply() = default;
    virtual void execute(Memory& memory) override;

    template<typename ISource>
    static std::unique_ptr<Multiply> from_source(ISource& source, int modes) {
        return Instruction::from_source<Multiply>(source, modes, 3);
    }
};

class Input : public Instruction {
public:
    Input(Pointer& pointer, Parameters parameters);
    virtual ~Input() = default;
    virtual void execute(Memory& memory) override;

    template<typename ISource>
    static std::unique_ptr<Input> from_source(ISource& source, int modes) {
        return Instruction::from_source<Input>(source, modes, 1);
    }
};

class Output : public Instruction {
public:
    Output(Pointer& pointer, Parameters parameters);
    virtual ~Output() = default;
    virtual void execute(Memory& memory) override;

    template<typename ISource>
    static std::unique_ptr<Output> from_source(ISource& source, int modes) {
        return Instruction::from_source<Output>(source, modes, 1);
    }
};

class JumpTrue : public Instruction {
public:
    JumpTrue(Pointer& pointer, Parameters parameters);
    virtual ~JumpTrue() = default;
    virtual void execute(Memory& memory) override;

    template<typename ISource>
    static std::unique_ptr<JumpTrue> from_source(ISource& source, int modes) {
        return Instruction::from_source<JumpTrue>(source, modes, 2);
    }
};

class JumpFalse : public Instruction {
public:
    JumpFalse(Pointer& pointer, Parameters parameters);
    virtual ~JumpFalse() = default;
    virtual void execute(Memory& memory) override;

    template<typename ISource>
	static std::unique_ptr<JumpFalse> from_source(ISource& source, int modes) {
		return Instruction::from_source<JumpFalse>(source, modes, 2);
	}
};

class LessThan : public Instruction {
public:
    LessThan(Pointer& pointer, Parameters parameters);
	virtual ~LessThan() = default;
	virtual void execute(Memory& memory) override;

	template<typename ISource>
	static std::unique_ptr<LessThan> from_source(ISource& source, int modes) {
		return Instruction::from_source<LessThan>(source, modes, 3);
	}
};

class Equal : public Instruction {
public:
    Equal(Pointer& pointer, Parameters parameters);
	virtual ~Equal() = default;
	virtual void execute(Memory& memory) override;

	template<typename ISource>
	static std::unique_ptr<Equal> from_source(ISource& source, int modes) {
		return Instruction::from_source<Equal>(source, modes, 3);
	}
};

class Stop : public Instruction {
public:
    Stop(Pointer& pointer, Parameters parameters);
    virtual ~Stop() = default;
    virtual void execute(Memory& memory) override;

    template<typename ISource>
    static std::unique_ptr<Stop> from_source(ISource& source, int modes) {
        return Instruction::from_source<Stop>(source, modes, 0);
    }
};

class Asteroids : public Day {
public:
    virtual void part_01() override;
    virtual void part_02() override;

    static std::unique_ptr<Day> create();
    static std::string name();

private:
    void run_program_from_source(std::string_view source);
    void parse_source_to_memory(std::string_view source);

private:
    static bool s_registered;
    std::vector<std::unique_ptr<Instruction>> program;
    std::vector<int> memory;
};

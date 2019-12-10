#pragma once

#include <map>
#include <deque>

#include "../day.hpp"

class Parameter;

using Parameters = std::vector<Parameter>;
using Memory = std::vector<int>;
using Pointer = Memory::iterator;
using Data = std::deque<int>;

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
    virtual void execute(Memory& memory, Data& input, Data& output) = 0;

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

    Pointer& m_pointer;
    std::vector<Parameter> m_parameters;
};

class Add: public Instruction {
public:
    Add(Pointer& pointer, Parameters parameters);
    virtual ~Add() = default;
    virtual void execute(Memory& memory, Data& input, Data& output) override;

    template<typename ISource>
    static std::unique_ptr<Add> from_source(ISource& source, int modes) {
        return Instruction::from_source<Add>(source, modes, 3);
    }
};

class Multiply : public Instruction {
public:
    Multiply(Pointer& pointer, Parameters parameters);
    virtual ~Multiply() = default;
    virtual void execute(Memory& memory, Data& input, Data& output) override;

    template<typename ISource>
    static std::unique_ptr<Multiply> from_source(ISource& source, int modes) {
        return Instruction::from_source<Multiply>(source, modes, 3);
    }
};

class Input : public Instruction {
public:
    Input(Pointer& pointer, Parameters parameters);
    virtual ~Input() = default;
    virtual void execute(Memory& memory, Data& input, Data& output) override;

    template<typename ISource>
    static std::unique_ptr<Input> from_source(ISource& source, int modes) {
        return Instruction::from_source<Input>(source, modes, 1);
    }
};

class Output : public Instruction {
public:
    Output(Pointer& pointer, Parameters parameters);
    virtual ~Output() = default;
    virtual void execute(Memory& memory, Data& input, Data& output) override;

    template<typename ISource>
    static std::unique_ptr<Output> from_source(ISource& source, int modes) {
        return Instruction::from_source<Output>(source, modes, 1);
    }
};

class JumpTrue : public Instruction {
public:
    JumpTrue(Pointer& pointer, Parameters parameters);
    virtual ~JumpTrue() = default;
    virtual void execute(Memory& memory, Data& input, Data& output) override;

    template<typename ISource>
    static std::unique_ptr<JumpTrue> from_source(ISource& source, int modes) {
        return Instruction::from_source<JumpTrue>(source, modes, 2);
    }
};

class JumpFalse : public Instruction {
public:
    JumpFalse(Pointer& pointer, Parameters parameters);
    virtual ~JumpFalse() = default;
    virtual void execute(Memory& memory, Data& input, Data& output) override;

    template<typename ISource>
	static std::unique_ptr<JumpFalse> from_source(ISource& source, int modes) {
		return Instruction::from_source<JumpFalse>(source, modes, 2);
	}
};

class LessThan : public Instruction {
public:
    LessThan(Pointer& pointer, Parameters parameters);
	virtual ~LessThan() = default;
	virtual void execute(Memory& memory, Data& input, Data& output) override;

	template<typename ISource>
	static std::unique_ptr<LessThan> from_source(ISource& source, int modes) {
		return Instruction::from_source<LessThan>(source, modes, 3);
	}
};

class Equal : public Instruction {
public:
    Equal(Pointer& pointer, Parameters parameters);
	virtual ~Equal() = default;
	virtual void execute(Memory& memory, Data& input, Data& output) override;

	template<typename ISource>
	static std::unique_ptr<Equal> from_source(ISource& source, int modes) {
		return Instruction::from_source<Equal>(source, modes, 3);
	}
};

class Stop : public Instruction {
public:
    Stop(Pointer& pointer, Parameters parameters);
    virtual ~Stop() = default;
    virtual void execute(Memory& memory, Data& input, Data& output) override;

    template<typename ISource>
    static std::unique_ptr<Stop> from_source(ISource& source, int modes) {
        return Instruction::from_source<Stop>(source, modes, 0);
    }
};

class Asteroids : public Day {
public:
    virtual std::string part_01() override;
    virtual std::string part_02() override;

    static std::unique_ptr<Day> create();
    static std::string name();

protected:
    Data run_program_from_source(const std::string& source, Data& input);
    void parse_source_to_memory(const std::string& source);
    std::vector<int> m_memory;

private:
    static bool s_registered;
    std::string src = "3,225,1,225,6,6,1100,1,238,225,104,0,1101,69,55,225,1001,144,76,224,101,-139,224,224,4,224,1002,223,8,223,1001,224,3,224,1,223,224,223,1102,60,49,225,1102,51,78,225,1101,82,33,224,1001,224,-115,224,4,224,1002,223,8,223,1001,224,3,224,1,224,223,223,1102,69,5,225,2,39,13,224,1001,224,-4140,224,4,224,102,8,223,223,101,2,224,224,1,224,223,223,101,42,44,224,101,-120,224,224,4,224,102,8,223,223,101,3,224,224,1,223,224,223,1102,68,49,224,101,-3332,224,224,4,224,1002,223,8,223,1001,224,4,224,1,224,223,223,1101,50,27,225,1102,5,63,225,1002,139,75,224,1001,224,-3750,224,4,224,1002,223,8,223,1001,224,3,224,1,223,224,223,102,79,213,224,1001,224,-2844,224,4,224,102,8,223,223,1001,224,4,224,1,223,224,223,1,217,69,224,1001,224,-95,224,4,224,102,8,223,223,1001,224,5,224,1,223,224,223,1102,36,37,225,1101,26,16,225,4,223,99,0,0,0,677,0,0,0,0,0,0,0,0,0,0,0,1105,0,99999,1105,227,247,1105,1,99999,1005,227,99999,1005,0,256,1105,1,99999,1106,227,99999,1106,0,265,1105,1,99999,1006,0,99999,1006,227,274,1105,1,99999,1105,1,280,1105,1,99999,1,225,225,225,1101,294,0,0,105,1,0,1105,1,99999,1106,0,300,1105,1,99999,1,225,225,225,1101,314,0,0,106,0,0,1105,1,99999,1107,677,677,224,102,2,223,223,1006,224,329,1001,223,1,223,1108,677,677,224,1002,223,2,223,1006,224,344,1001,223,1,223,107,226,226,224,1002,223,2,223,1006,224,359,101,1,223,223,1008,226,226,224,102,2,223,223,1005,224,374,1001,223,1,223,1107,226,677,224,1002,223,2,223,1006,224,389,1001,223,1,223,1008,677,226,224,1002,223,2,223,1005,224,404,1001,223,1,223,7,677,226,224,102,2,223,223,1005,224,419,1001,223,1,223,1008,677,677,224,1002,223,2,223,1006,224,434,1001,223,1,223,108,226,226,224,102,2,223,223,1006,224,449,1001,223,1,223,108,677,677,224,102,2,223,223,1006,224,464,1001,223,1,223,107,226,677,224,1002,223,2,223,1005,224,479,101,1,223,223,1108,226,677,224,1002,223,2,223,1006,224,494,1001,223,1,223,107,677,677,224,1002,223,2,223,1006,224,509,101,1,223,223,7,677,677,224,102,2,223,223,1006,224,524,1001,223,1,223,1007,226,677,224,1002,223,2,223,1005,224,539,1001,223,1,223,8,226,677,224,1002,223,2,223,1005,224,554,101,1,223,223,8,677,677,224,102,2,223,223,1005,224,569,101,1,223,223,7,226,677,224,102,2,223,223,1006,224,584,1001,223,1,223,1007,226,226,224,102,2,223,223,1006,224,599,1001,223,1,223,1107,677,226,224,1002,223,2,223,1006,224,614,1001,223,1,223,1108,677,226,224,1002,223,2,223,1005,224,629,1001,223,1,223,1007,677,677,224,102,2,223,223,1006,224,644,1001,223,1,223,108,226,677,224,102,2,223,223,1005,224,659,101,1,223,223,8,677,226,224,1002,223,2,223,1006,224,674,1001,223,1,223,4,223,99,226";
};

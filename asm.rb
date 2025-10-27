class MipsMicroAssembler
    def initialize
      @binary_codes = []
      @current_address = 0
      @pending_branches = []
      @pending_jumps = []
      @source_lines = []
    end
  
    REGISTERS = 
    {
      zero: 0, c: 1, v0: 2, v1: 3, a0: 4, a1: 5, a2: 6, a3: 7,
      t0: 8, t1: 9, t2: 10, t3: 11, t4: 12, t5: 13, t6: 14, t7: 15,
      c0: 16, c1: 17, c2: 18, c3: 19, c4: 20, c5: 21, c6: 22, c7: 23,
      t8: 24, t9: 25, d0: 26, d1: 27, d2: 28, ad3: 29, d4: 30, d5: 31
    }
  
    OPCODES = 
    {
      J:     0b010000,
      CLS:   0b000000,
      BEQ:   0b001011,
      OR:    0b000000,
      RORI:  0b100011,
      SELC:  0b000000,
      LDP:   0b000010,
      LD:    0b101100,
      SLTI:  0b110011,
      ST:    0b000110,
      BNE:   0b001100,
      ADD:   0b000000,
      USAT:  0b100110
    }
  
    FUNCTS = 
    {
      CLS:    0b101010,
      OR:     0b111001,
      SELC:   0b011010,
      ADD:    0b110010,
      SYSCALL: 0b110001
    }
  
    def j(target)
      opcode = OPCODES[:J]
      target_address = target >> 2
      emit_instruction((opcode << 26) | (target_address & 0x3FFFFFF), "j 0x#{target.to_s(16)}")
    end
  
    def cls(rd, rs)
      opcode = OPCODES[:CLS]
      funct = FUNCTS[:CLS]
      emit_instruction((opcode << 26) | (REGISTERS[rd] << 21) | (REGISTERS[rs] << 16) | funct, "cls #{rd}, #{rs}")
    end
  
    def beq(rs, rt, target)
      opcode = OPCODES[:BEQ]
      target_address = (target - (@current_address + 4)) >> 2
      emit_instruction((opcode << 26) | (REGISTERS[rs] << 21) | (REGISTERS[rt] << 16) | (target_address & 0xFFFF), "beq #{rs}, #{rt}, 0x#{target.to_s(16)}")
    end
  
    def or(rd, rs, rt)
      opcode = OPCODES[:OR]
      funct = FUNCTS[:OR]
      emit_instruction((opcode << 26) | (REGISTERS[rs] << 21) | (REGISTERS[rt] << 16) | (REGISTERS[rd] << 11) | funct, "or #{rd}, #{rs}, #{rt}")
    end
  
    def rori(rd, rs, imm5)
      opcode = OPCODES[:RORI]
      emit_instruction((opcode << 26) | (REGISTERS[rd] << 21) | (REGISTERS[rs] << 16) | ((imm5 & 0x1F) << 11), "rori #{rd}, #{rs}, #{imm5}")
    end
  
    def selc(rd, rs1, rs2)
      opcode = OPCODES[:SELC]
      funct = FUNCTS[:SELC]
      emit_instruction((opcode << 26) | (REGISTERS[rd] << 21) | (REGISTERS[rs1] << 16) | (REGISTERS[rs2] << 11) | funct, "selc #{rd}, #{rs1}, #{rs2}")
    end
  
    def ldp(rt1, rt2, base, offset)
      opcode = OPCODES[:LDP]
      emit_instruction((opcode << 26) | (REGISTERS[base] << 21) | (REGISTERS[rt1] << 16) | (REGISTERS[rt2] << 11) | (offset & 0x7FF), "ldp #{rt1}, #{rt2}, #{offset}(#{base})")
    end
  
    def ld(rt, base, offset)
      opcode = OPCODES[:LD]
      emit_instruction((opcode << 26) | (REGISTERS[base] << 21) | (REGISTERS[rt] << 16) | (offset & 0xFFFF), "ld #{rt}, #{offset}(#{base})")
    end
  
    def slti(rt, rs, imm)
      opcode = OPCODES[:SLTI]
      emit_instruction((opcode << 26) | (REGISTERS[rs] << 21) | (REGISTERS[rt] << 16) | (imm & 0xFFFF), "slti #{rt}, #{rs}, #{imm}")
    end
  
    def st(rt, base, offset)
      opcode = OPCODES[:ST]
      emit_instruction((opcode << 26) | (REGISTERS[base] << 21) | (REGISTERS[rt] << 16) | (offset & 0xFFFF), "st #{rt}, #{offset}(#{base})")
    end
  
    def bne(rs, rt, target)
      opcode = OPCODES[:BNE]
      target_address = (target - (@current_address + 4)) >> 2
      emit_instruction((opcode << 26) | (REGISTERS[rs] << 21) | (REGISTERS[rt] << 16) | (target_address & 0xFFFF), "bne #{rs}, #{rt}, 0x#{target.to_s(16)}")
    end
  
    def add(rd, rs, rt)
      opcode = OPCODES[:ADD]
      funct = FUNCTS[:ADD]
      instruction = (opcode << 26) | (REGISTERS[rs] << 21) | (REGISTERS[rt] << 16) | (REGISTERS[rd] << 11) | funct
      emit_instruction(instruction, "add #{rd}, #{rs}, #{rt}")
    end
  
    def usat(rd, rs, imm5)
      opcode = OPCODES[:USAT]
      emit_instruction((opcode << 26) | (REGISTERS[rd] << 21) | (REGISTERS[rs] << 16) | ((imm5 & 0x1F) << 11), "usat #{rd}, #{rs}, #{imm5}")
    end
  
    def syscall
      opcode = OPCODES[:ADD]
      funct = FUNCTS[:SYSCALL]
      emit_instruction((opcode << 26) | funct, "syscall")
    end
  
    private
  
    def emit_instruction(instruction, source = nil)
      @binary_codes << ((instruction >> 24) & 0xFF)
      @binary_codes << ((instruction >> 16) & 0xFF)
      @binary_codes << ((instruction >> 8) & 0xFF)
      @binary_codes << (instruction & 0xFF)
      if source != nil
        @source_lines << "    #{source}"
      end
      @current_address += 4
    end
  
    public
  
    def save(filename)
      File.binwrite(filename, @binary_codes.pack('C*'))
      puts "File '#{filename}' created with #{@binary_codes.size} bytes (#{@binary_codes.size / 4} instructions)"
    end
  
    def dump_source
      puts "prog do"
      @source_lines.each do |line|
        puts line
      end
      puts "end"
      puts "hlt"
    end
end

def generate_fibonacci_program(n)
    assembler = MipsMicroAssembler.new
  
    assembler.slti :t2, :zero, 1
  
    assembler.add :t0, :zero, :zero
    assembler.add :t1, :zero, :t2
  
    (n - 1).times do |i|
      assembler.add :t2, :t0, :t1 
      assembler.add :t0, :zero, :t1
      assembler.add :t1, :zero, :t2 
    end
  
    assembler.add :v0, :zero, :t2
  
    assembler.dump_source
    assembler.save("build/fibonacci.bin")
    
    puts  "n=#{n}"
    puts "F(#{n}) = #{calculate_fibonacci(n)}"
  end
  
  def calculate_fibonacci(n)
    return 0 if n == 0
    return 1 if n == 1
    
    a, b = 0, 1
    (n - 1).times do
      a, b = b, a + b
    end
    b
  end
  
  generate_fibonacci_program(7)
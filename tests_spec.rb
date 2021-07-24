describe 'database' do
    before do
      `rm -rf ./.data/test.db`
    end

    def run_script(commands)
        raw_output = nil
        IO.popen("./build/main ./.data/test.db", "r+") do |pipe|
            commands.each do |command|
                pipe.puts command
            end

            pipe.close_write

            # Read entire output
            raw_output = pipe.gets(nil)
        end
        raw_output.split("\n")
    end

    it 'inserts and retrives a row' do
        result = run_script([
            "insert 1 anders anders@favn.com",
            "select",
            ".exit"
        ])
        expect(result).to match_array([
            "db > Executed.",
            "db > Row(1, anders, anders@favn.com)",
            "db > Exiting...",
        ])
    end

    it 'prints error message when table is full' do
        script = (0..15).map do |i| 
            "insert #{i} user#{i} person#{i}@example.com"
        end

        script << ".exit"

        result = run_script(script)
        # result.each { |s| puts s}
        expect(result[-2]).to eq("db > Error: table full")
    end

    it 'allows inserting strings that are the maximum length' do
        long_username = "a"*32
        long_email = "a"*255
        script = [
            "insert 1 #{long_username} #{long_email}",
            "select",
            ".exit",
        ]
        result = run_script(script)
        expect(result).to match_array([
            "db > Executed.",
            "db > Row(1, #{long_username}, #{long_email})",
            "db > Exiting..."
        ])
    end

    it 'keeps data after closing connection' do
        result1 = run_script([
          "insert 1 user1 person1@example.com",
          ".exit",
        ])
        expect(result1).to match_array([
          "db > Executed.",
          "db > Exiting...",
        ])
        result2 = run_script([
          "select",
          ".exit",
        ])
        expect(result2).to match_array([
          "db > Row(1, user1, person1@example.com)",
          "db > Exiting...",
        ])
    end

    it 'prints constants' do
        script = [
            ".constants",
            ".exit",
        ]
        result = run_script(script)

        expect(result).to match_array([
            "db > Constants:",
            "ROW_SIZE: 307",
            "COMMON_NODE_HEADER_SIZE: 6",
            "LEAF_NODE_HEADER_SIZE: 10",
            "LEAF_NODE_CELL_SIZE: 311",
            "LEAF_NODE_SPACE_FOR_CELLS: 4086",
            "LEAF_NODE_MAX_CELLS: 13",
            "db > Exiting...",
        ])
    end

    it 'allows printing out the structure of a one-node btree' do
        script = [3, 1, 2].map do |i|
            "insert #{i} user#{i} person#{i}@example.com"
        end
        script << ".btree"
        script << ".exit"
        result = run_script(script)

        expect(result).to match_array([
            "db > Executed.",
            "db > Executed.",
            "db > Executed.",
            "db > Tree:",
            "- leaf (size 3)",
            " - 1",
            " - 2",
            " - 3",
            "db > Exiting..."
        ])
    end

    it 'prints an error message if there is a duplicate id' do
        script = [
            "insert 1 user1 person1@example.com",
            "insert 1 user1 person1@example.com",
            "select",
            ".exit",
        ]
        result = run_script(script)
        expect(result).to match_array([
            "db > Executed.",
            "db > Error: Duplicate key.",
            "db > Row(1, user1, person1@example.com)",
            "db > Exiting...",
        ])
    end


    it 'allows printing out the structure of a 3-leaf-node btree' do
        script = (1..14).map do |i|
            "insert #{i} user#{i} person#{i}@example.com"
        end
        script << ".btree"
        script << "insert 15 user15 person15@example.com"
        script << ".exit"
        # script.each { |s| puts s}
        result = run_script(script)

        # result.each { |s| puts s}
        # puts result.length
        # puts result[0...(result.length)]
        # expect(["true"]).to match_array(["false"]);


        expect(result[14...(result.length)]).to match_array([
            "db > Tree:",
            "- internal (size 1)",
            "  - leaf (size 7)",
            "    - 1",
            "    - 2",
            "    - 3",
            "    - 4",
            "    - 5",
            "    - 6",
            "    - 7",
            "  - key 7",
            "  - leaf (size 7)",
            "    - 8",
            "    - 9",
            "    - 10",
            "    - 11",
            "    - 12",
            "    - 13",
            "    - 14",
            "db > Need to implement searching an internal node",
        ])
    end
end
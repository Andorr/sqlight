describe 'database' do
    before do
      `rm -rf ./.data/test.db`
    end

    def run_script(commands)
        raw_output = nil
        IO.popen("./build/main ./.data/test.db", "r+") do |pipe|
            commands.each do |command|
                begin
                    puts "Command: #{command}"
                    pipe.puts command
                rescue Errno::EPIPE
                    break
                end
            end

            pipe.close_write

            # Read entire output
            raw_output = pipe.gets(nil)
            puts "Raw:\n#{raw_output}"
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
        script = (0..1400).map do |i| 
            "insert #{i} user#{i} person#{i}@example.com"
        end

        script << ".exit"

        result = run_script(script)
        puts result.length()
        result.each { |s| puts s }
        expect(result.last(2)).to match_array([
            "db > Executed.",
            "db > Need to implement splitting internal node",
        ])
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
            "LEAF_NODE_HEADER_SIZE: 14",
            "LEAF_NODE_CELL_SIZE: 311",
            "LEAF_NODE_SPACE_FOR_CELLS: 4082",
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
        result = run_script(script)

        puts result.length()

        expect(result[14...(result.length)]).to match_array([
            "db > Tree:",
            "- internal (size 1)",
            " - leaf (size 7)",
            "  - 1",
            "  - 2",
            "  - 3",
            "  - 4",
            "  - 5",
            "  - 6",
            "  - 7",
            " - key 7",
            " - leaf (size 7)",
            "  - 8",
            "  - 9",
            "  - 10",
            "  - 11",
            "  - 12",
            "  - 13",
            "  - 14",
            "db > Executed.",
            "db > ",
        ])
    end

    it 'prints all rows in a multi-level tree' do
        script = []
        (1..15).each do |i|
          script << "insert #{i} user#{i} person#{i}@example.com"
        end
        script << "select"
        script << ".exit"
        result = run_script(script)

        expect(result[15...result.length]).to match_array([
          "db > (1, user1, person1@example.com)",
          "Row(2, user2, person2@example.com)",
          "Row(3, user3, person3@example.com)",
          "Row(4, user4, person4@example.com)",
          "Row(5, user5, person5@example.com)",
          "Row(6, user6, person6@example.com)",
          "Row(7, user7, person7@example.com)",
          "Row(8, user8, person8@example.com)",
          "Row(9, user9, person9@example.com)",
          "Row(10, user10, person10@example.com)",
          "Row(11, user11, person11@example.com)",
          "Row(12, user12, person12@example.com)",
          "Row(13, user13, person13@example.com)",
          "Row(14, user14, person14@example.com)",
          "Row(15, user15, person15@example.com)",
          "Executed.", "db > ",
        ])
    end

    it 'allows printing out the structure of a 4-leaf-node btree' do
        script = [
          "insert 18 user18 person18@example.com",
          "insert 7 user7 person7@example.com",
          "insert 10 user10 person10@example.com",
          "insert 29 user29 person29@example.com",
          "insert 23 user23 person23@example.com",
          "insert 4 user4 person4@example.com",
          "insert 14 user14 person14@example.com",
          "insert 30 user30 person30@example.com",
          "insert 15 user15 person15@example.com",
          "insert 26 user26 person26@example.com",
          "insert 22 user22 person22@example.com",
          "insert 19 user19 person19@example.com",
          "insert 2 user2 person2@example.com",
          "insert 1 user1 person1@example.com",
          "insert 21 user21 person21@example.com",
          "insert 11 user11 person11@example.com",
          "insert 6 user6 person6@example.com",
          "insert 20 user20 person20@example.com",
          "insert 5 user5 person5@example.com",
          "insert 8 user8 person8@example.com",
          "insert 9 user9 person9@example.com",
          "insert 3 user3 person3@example.com",
          "insert 12 user12 person12@example.com",
          "insert 27 user27 person27@example.com",
          "insert 17 user17 person17@example.com",
          "insert 16 user16 person16@example.com",
          "insert 13 user13 person13@example.com",
          "insert 24 user24 person24@example.com",
          "insert 25 user25 person25@example.com",
          "insert 28 user28 person28@example.com",
          ".btree",
          ".exit",
        ]
        result = run_script(script)
    end
end
describe 'database' do
    def run_script(commands)
        raw_output = nil
        IO.popen("./build/main", "r+") do |pipe|
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
        script = (1..4900).map do |i| 
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
end
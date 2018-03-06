import java.io.IOException;
import java.util.*;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

//Author: Thomas Noelcke
//Assignment: CS 440 HW 7


public class PageCount {

	public static class TokenizerMapper
			extends Mapper<Object, Text, Text, IntWritable>{
      
      @Override
			public void map(Object key, Text value, Context output) 
                  throws IOException, InterruptedException {
          System.out.println(value);
			    String[] words = value.toString().split(" ");
          if(words.length >= 3){
            int num = Integer.parseInt(words[1]);
            String valid = words[words.length - 1];
            if(valid.equals("T")) {
              output.write(new Text(words[0]), new IntWritable(num));
            } 
          }
      }
	}

	public static class IntSumReducer
			extends Reducer<Text,IntWritable,Text,IntWritable> {
			private IntWritable result = new IntWritable();

			public void reduce(Text key, Iterable<IntWritable> values, Context output) 
        throws IOException, InterruptedException {
          int visitCount = 0;

          for(IntWritable value:values){
            visitCount += value.get();
          }

          output.write(key, new IntWritable(visitCount));
      }
	}

	public static int main(String[] args) throws Exception
  {
		if(args.length != 2){
      System.out.println("usage: [input] [output]");
      System.exit(-1);
    }
    Configuration conf = new Configuration();
    Job job = Job.getInstance(conf, "page count");

    job.setJarByClass(PageCount.class);
    job.setMapperClass(TokenizerMapper.class);
    
    job.setCombinerClass(IntSumReducer.class);
    job.setReducerClass(IntSumReducer.class);
    
    job.setOutputKeyClass(Text.class);
    job.setOutputValueClass(IntWritable.class);
    
    FileInputFormat.addInputPath(job, new Path(args[0]));
    FileOutputFormat.setOutputPath(job, new Path(args[1]));

    return (job.waitForCompletion(true) ? 0 : 1);
	}
}

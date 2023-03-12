## Does the presence of the Content-Length Header depend on the Transfer-encoding Header:

The Transfer-encoding header, comes with a POST request and specifies the form of encoding used to safely transfer the payload body, to the user.

If transfer-encoding was specified as **chunked**, in an HTTP message. the body of the message consists of an **unspecified number of chunks**, a **terminating chunk**, **trailer**, and a **CRLF sequence**.

## Use with Compression 
If both compression and chunked are enabled, then the content stream is first compressed, then chunked, so the chunk encoding itself is not compressed, and the data in each chunk is not compressed individually. The remote endpoints then decodes the stream by concatenating the chunks and uncompressing the results.

## Can I read more than the chunk size

If you're receiving an HTTP response with Transfer-Encoding: chunked, you should not read more data than the size of the current chunk indicates. Reading beyond the end of a chunk can result in data corruption or lead to incorrect interpretation of subsequent chunks.

## Where does that next chunk dwells when I'm receiving the current chunk ?

## for the note, you added at the end, I would like to inquire you about this, When I use the recv() syscall to read data from a socket, into a buffer, will I receive only one chunk or not necesserly ?

When you use the recv() system call to read data from a socket, you may receive only a part of a chunk, a whole chunk, or multiple chunks depending on the size of the chunks being sent and the size of the buffer you are using to receive the data.
The recv() function reads data from a socket into a buffer of a specified size. The function returns the number of bytes that were actually received, which may be less than the size of the buffer. If the buffer is smaller than a full chunk, the function will receive only part of the chunk. If the buffer is larger than a chunk, the function may receive multiple chunks in a single call.

Therefore, it's important to keep track of the state of the incoming data, and to parse the data carefully to ensure that you are processing complete chunks of data rather than partial chunks or multiple chunks mixed together.
One common approach to handling chunked data is to read the size indicator for each chunk first, and then use that size to read the corresponding chunk of data. This ensures that you are reading complete chunks rather than partial chunks. You can then process each chunk as needed before moving on to the next one.

## Will I get the same file that were Sent (is append well working)?

In general, if the server receives all of the chunks of the video correctly and writes them to the file in append mode, the resulting file should be the same as the original video file that was sent in the POST request.

However, there are a few factors that could affect the accuracy of the resulting file:

Data corruption during transmission: If any of the chunks are corrupted or lost during transmission, this could result in errors in the final video file.

Incomplete chunks: If the server reads partial chunks or misses some chunks due to buffer overflow or other issues, this could result in missing or incomplete data in the final video file.

Encoding issues: If the server is responsible for encoding the video in a specific format (e.g. converting a raw video stream to an MP4 file), errors in the encoding process could result in differences between the original video file and the final file.

To minimize the risk of errors, it's important to carefully handle the chunked POST request, ensure that all chunks are received correctly and written to the file in the correct order, and perform any necessary error checking and correction. It's also important to test the server thoroughly to ensure that it can handle a variety of different video formats and sizes without issues.

### LOGIC 1
I will read the data, into a buffer, till 
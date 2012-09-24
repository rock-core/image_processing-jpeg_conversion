require 'jpeg_conversion_ext'

module Conversion
    class JpegConversion
        def from_file(file, width, height)
            frame = Types::Base::Samples::Frame::Frame.new
            frame.size.height = height
            frame.size.width = width
            frame.frame_mode = :MODE_JPEG
            frame.image = do_from_file(file,width,height)
            return frame
        end

        def load_jpeg(*args)
            raise RuntimeError, "load_jpeg is not supported any more. Use from_file(filename, image_width, image_height)"
        end
    end
end

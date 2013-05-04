"""File containing the beginning stages of our algorithm code-5 minute blocks"""
import config
import socket
import re
import collections

class Algorithm(object):
    
    def take_from_list(self, amount):
        """Function to take variable number of elements off of an ordered dict"""
        """Returns list of entries with size amount"""
        out_list = []
        for x in range(1, amount):
            out_list = out_list.append(next(self))
